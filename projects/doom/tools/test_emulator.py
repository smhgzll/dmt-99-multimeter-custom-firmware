#!/usr/bin/env python3
"""Run the actual linked RV32 BIN through the copied board/panel emulator.

The parent project is only read (NOR fixture); all reports/screenshots are local.
No CPU instructions are replaced with host renderer calls.
"""
import argparse,ctypes,hashlib,json,struct,sys
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
sys.dont_write_bytecode=True
sys.path.insert(0,str(ROOT/'tools/reference'))
from fw_emulator import Ch32Emulator

class SoundEmulator(Ch32Emulator):
    """Observe timer/GPIO writes; execute all original emulator operations."""
    def __init__(self,*args,**kwargs):
        self.tones=[]
        super().__init__(*args,**kwargs)
    def _mmio_write(self,uc,offset,size,value,data):
        super()._mmio_write(uc,offset,size,value,data)
        if offset==0 and value&1 and self.regs.get(0x40000038,0):
            psc=self.regs.get(0x40000028,0)+1;counts=self.regs.get(0x4000002c,0)+1
            duty=self.regs[0x40000038]/counts
            assert self.regs.get(0x40000020,0)&0x10,'TIM2 CH2 disabled'
            assert (self.regs.get(0x40010800,0)>>4)&15==11,'PA1 not AF push-pull'
            assert .49<=duty<=.51,('unexpected duty',duty)
            self.tones.append({'hz':144_000_000/psc/counts,'psc':psc-1,'arr':counts-1,'duty':duty})

def main():
    from unicorn import UC_HOOK_CODE,UC_HOOK_MEM_WRITE
    from unicorn.riscv_const import UC_RISCV_REG_A1,UC_RISCV_REG_A2
    ap=argparse.ArgumentParser();ap.add_argument('--out',type=Path,default=ROOT/'build/emulator');a=ap.parse_args()
    a.out.mkdir(parents=True,exist_ok=True)
    lib=ctypes.CDLL(str(ROOT/'build/libdoom-host.so'))
    lib.doom_host_pixels.restype=ctypes.POINTER(ctypes.c_uint16)
    lib.doom_host_input.argtypes=[ctypes.c_char];lib.doom_host_advance.argtypes=[ctypes.c_uint32]
    lib.doom_host_motion.argtypes=[ctypes.c_int32,ctypes.c_int32]
    lib.doom_get_tick.restype=ctypes.c_uint32
    reports=[]
    for label,leftovers,hse in [('clean',False,True),('leftovers',True,True),('hsi',False,False),('held-power',False,True)]:
        e=SoundEmulator(bin_path=ROOT/'build/dmt99-doom.bin',
            flash_dump=ROOT.parents[1]/'flashdump/dmt99_flash_1.bin',bootloader_leftovers=leftovers,hse_ok=hse)
        initial_nor=hashlib.sha256(e.nor.data).hexdigest();initial_settings=bytes(e.settings)
        lib.doom_host_init();e.seen_frames=0;e.capture_frame=False;e.compare_enabled=True;e.native_frames=0;e.actions=[];e.motions=[]
        if label=='held-power':e.pressed.add('PC2')
        def native_time():
            target=struct.unpack('<I',e.uc.mem_read(e.symbols['doom_simulation_tick'],4))[0]
            assert lib.doom_get_tick()<=target,(lib.doom_get_tick(),target)
            while lib.doom_get_tick()<target:lib.doom_host_advance(min(500,(target-lib.doom_get_tick())*100))
        def input_hook(uc,address,size,data):
            e.actions.append(chr(uc.reg_read(UC_RISCV_REG_A1)&255))
            if e.compare_enabled:
                native_time();lib.doom_host_input(bytes([uc.reg_read(UC_RISCV_REG_A1)&255]))
        def render_hook(uc,address,size,data):
            if e.compare_enabled:native_time()
        def motion_hook(uc,address,size,data):
            move=ctypes.c_int32(uc.reg_read(UC_RISCV_REG_A1)).value
            turn=ctypes.c_int32(uc.reg_read(UC_RISCV_REG_A2)).value
            e.motions.append((move,turn))
            if e.compare_enabled:native_time();lib.doom_host_motion(move,turn)
        def completed(uc,access,address,size,value,data):
            if value<=e.seen_frames:return
            e.seen_frames=value
            if e.compare_enabled:
                native=hashlib.sha256(struct.pack('<76800H',*lib.doom_host_pixels()[:76800])).hexdigest()
                assert e.panel.checksum()==native,('RV32/native frame mismatch',value,e.panel.checksum(),native)
                e.native_frames+=1
            if e.capture_frame:e.capture_frame=False;uc.emu_stop()
        e.uc.hook_add(UC_HOOK_CODE,input_hook,begin=e.symbols['doom_app_input'],end=e.symbols['doom_app_input'])
        e.uc.hook_add(UC_HOOK_CODE,render_hook,begin=e.symbols['doom_app_draw'],end=e.symbols['doom_app_draw'])
        e.uc.hook_add(UC_HOOK_CODE,motion_hook,begin=e.symbols['doom_app_motion'],end=e.symbols['doom_app_motion'])
        counter=e.symbols['doom_frame_count']
        e.uc.hook_add(UC_HOOK_MEM_WRITE,completed,begin=counter,end=counter+3)
        def frame(name):
            before=e.seen_frames;e.capture_frame=True
            e.run(seconds=10,until=lambda _:e.seen_frames>before,max_instr=200_000_000)
            assert e.seen_frames>before,('no frame',e.fault,e.pc)
            e.panel.save(a.out/(name+'.png'))
        def press(pin,name,hold=.12,expected=None):
            e.run(seconds=.25);before=len(e.actions);e.press(pin,hold=hold)
            e.run(seconds=4,until=lambda _:len(e.actions)>before,max_instr=200_000_000)
            assert len(e.actions)>before,('key not processed',pin,e.actions)
            if expected is not None:assert e.actions[-1]==expected,(pin,e.actions[-1],expected)
            frame(name);e.run(seconds=hold+.15)
        def movement(pin,name):
            e.run(seconds=.2);before=len(e.motions);e.press(pin,hold=.12)
            e.run(seconds=3,until=lambda _:len(e.motions)>before,max_instr=200_000_000)
            assert len(e.motions)>before,('no motion',pin)
            m,t=e.motions[-1]
            assert {'PC11':m>0,'PC10':m<0,'PC12':t>0,'PC9':t<0}[pin],(pin,m,t)
            frame(name);e.run(seconds=.3)
        def pause_combo(name):
            e.run(seconds=.2);before=len(e.actions);start=e.cycles
            e.press('PC8',hold=2.25);e.press('PC10',hold=2.25)
            e.run(seconds=1.8)
            assert 'p' not in e.actions[before:],'menu before 2s'
            e.run(seconds=1.0,until=lambda _:'p' in e.actions[before:],max_instr=200_000_000)
            assert e.actions[before:]==['p'],('chord fired or missed',e.actions[before:])
            assert e.cycles-start>=288_000_000
            frame(name);e.run(seconds=.4)
        frame(label+'-menu')
        result={'case':label,'initial_menu':e.panel.checksum()}
        if label=='clean':
            # Weapons menu: PISTOL -> SHOTGUN -> select -> start game.
            press('PC10','menu-weapons');press('PC8','weapons')
            press('PC10','shotgun');press('PC8','selected-shotgun')
            press('PC11','menu-new');press('PC8','game')
            for pin,name in [('PC9','right'),('PC12','left'),('PC11','forward'),('PC10','back')]:movement(pin,name)
            press('PC8','fire',expected='f')
            before_actions=len(e.actions);e.press('PC8',hold=2.3);e.run(seconds=2.6)
            assert e.actions[before_actions:] and set(e.actions[before_actions:])=={'f'},'solo fire opened menu'
            frame('held-fire')
            before=struct.unpack('<I',e.uc.mem_read(e.symbols['doom_simulation_tick'],4))[0]
            frame('idle-animation-1');frame('idle-animation-2')
            after=struct.unpack('<I',e.uc.mem_read(e.symbols['doom_simulation_tick'],4))[0]
            assert after>before,'world clock did not advance without keys'
            pause_combo('pause')
            paused=struct.unpack('<I',e.uc.mem_read(e.symbols['doom_simulation_tick'],4))[0]
            e.run(seconds=.5);assert struct.unpack('<I',e.uc.mem_read(e.symbols['doom_simulation_tick'],4))[0]==paused
            assert not e.buzzer_on()
            press('PC8','resume');assert struct.unpack('<I',e.uc.mem_read(e.symbols['doom_simulation_tick'],4))[0]>=paused
            # Target-only SRAM fixtures: do not claim native image equivalence
            # after replacing actor positions. The real BIN continues to run.
            e.compare_enabled=False
            classes=bytes(e.uc.mem_read(e.symbols['doom_enemy_class'],103))
            facings=bytes(e.uc.mem_read(e.symbols['doom_enemy_facing'],103))
            npc=next(i for i in range(103) if classes[i]==0 and facings[i]==4)
            def put(name,fmt,value,offset=0):e.uc.mem_write(e.symbols[name]+offset,struct.pack('<'+fmt,value))
            def fixture(x,y,ex,ey):
                e.pressed.clear();e.run(seconds=.2)
                put('px','f',x);put('py','f',y);put('angle_deg','h',0);put('player_health','h',100)
                e.uc.mem_write(e.symbols['enemy_alive'],bytes(13))
                e.uc.mem_write(e.symbols['enemy_state'],bytes(103))
                e.uc.mem_write(e.symbols['missiles'],bytes(160))
                e.uc.mem_write(e.symbols['enemy_timer'],bytes(103));e.uc.mem_write(e.symbols['enemy_cooldown'],bytes(103))
                e.uc.mem_write(e.symbols['detour_ticks'],bytes(103));put('weapon_cooldown','B',0)
                put('enemy_pos','h',ex,npc*4);put('enemy_pos','h',ey,npc*4+2)
                put('enemy_health','h',35,npc*2)
                e.uc.mem_write(e.symbols['enemy_alive']+npc//8,bytes([1<<(npc%8)]))
            fixture(96,-272,500,-272);put('angle_fraction','B',0);frame('enemy-idle')
            initial_x=struct.unpack('<h',e.uc.mem_read(e.symbols['enemy_pos']+npc*4,2))[0]
            frame('enemy-walk-1');frame('enemy-walk-2');frame('enemy-walk-3')
            moving_x=struct.unpack('<h',e.uc.mem_read(e.symbols['enemy_pos']+npc*4,2))[0]
            assert moving_x<initial_x,'idle enemy did not see/chase without input'
            e.run(seconds=2.0);frame('enemy-attack')
            assert struct.unpack('<h',e.uc.mem_read(e.symbols['player_health'],2))[0]<100,'visible enemy never attacked'
            prior_kills=struct.unpack('<H',e.uc.mem_read(e.symbols['kills'],2))[0]
            press('PC8','enemy-death',expected='f')
            assert struct.unpack('<H',e.uc.mem_read(e.symbols['kills'],2))[0]==prior_kills+1
            for i in range(3):frame('corpse-'+str(i))
            put('enemy_timer','B',1,npc);frame('corpse-removed')
            assert e.uc.mem_read(e.symbols['enemy_state']+npc,1)[0]==7
            fixture(1064,224,1112,224);e.run(seconds=1.0)
            assert e.uc.mem_read(e.symbols['enemy_state']+npc,1)[0]==0,'vision crossed the wall'
            assert struct.unpack('<h',e.uc.mem_read(e.symbols['player_health'],2))[0]==100
            press('PC8','wall-blocks-shot',expected='f')
            assert struct.unpack('<h',e.uc.mem_read(e.symbols['enemy_health']+npc*2,2))[0]==35,'shot crossed the wall'
            assert e.uc.mem_read(e.symbols['enemy_state']+npc,1)[0]==0,'gun noise crossed wall'
            for i in range(6):
                frame('wall-detour-'+str(i))
                x,y=struct.unpack('<hh',e.uc.mem_read(e.symbols['enemy_pos']+npc*4,4))
                if 116<y<332:assert x>=1100,('enemy crossed barrier',x,y)
            result['target_fixtures']={'autonomous_sight_chase_attack':True,'kill_and_corpse':True,
                                       'corpse_removed':True,'wall_blocks_vision_shot_and_noise':True,'collision_during_detour':True}
            pause_combo('options-menu')
            for i in range(3):press('PC10','option-'+str(i))
            press('PC8','god-on');assert e.uc.mem_read(e.symbols['god_mode'],1)[0]==1
            press('PC10','sound-option');press('PC8','sound-off');assert not e.buzzer_on()
            press('PC12','options-resume')
            fixture(96,-272,160,-272);put('enemy_state','B',1,npc);put('enemy_health','h',200,npc*2)
            before_tones=len(e.tones);e.run(seconds=2.0);frame('god-protects')
            assert struct.unpack('<h',e.uc.mem_read(e.symbols['player_health'],2))[0]==100
            press('PC8','muted-fire',expected='f');assert len(e.tones)==before_tones and not e.buzzer_on()
            result['target_fixtures']['god_and_sound_menu']=True
            e.press('PC2',hold=.12);e.run(seconds=.5);assert e.halted and not e.gpio['C']&2 and not e.buzzer_on()
            result.update({'weapons_menu':True,'autonomous_clock':True,'pause_freezes_clock':True,
                           'middle_always_fire':True,'two_second_middle_down_menu':True,'gpio_ramped_motion':True,
                           'power_off':True,'tones':e.tones})
        elif label=='held-power':
            assert e.gpio['C']&2 and not e.halted
            e.pressed.discard('PC2');e.run(seconds=.1);e.press('PC2',hold=.12);e.run(seconds=.4)
            assert e.halted and not e.gpio['C']&2
        assert not e.flash_ops and not e.flash_keys and not e.mode_keys and bytes(e.settings)==initial_settings
        assert hashlib.sha256(e.nor.data).hexdigest()==initial_nor and not e.tx_packets and not e.uart_tx
        assert not e.panel.out_of_range and not e.fault and not e.irq_log
        result.update({'frames':e.seen_frames,'native_frames_compared':e.native_frames,'fault':e.fault,'no_asic_tx':True,
                       'no_internal_flash_writes':True,'nor_unchanged':True,'panel_out_of_range':0,
                       'stack_used_bytes':6144-sum(4 for off in range(0,6144,4) if bytes(e.uc.mem_read(0x20006800+off,4))==struct.pack('<I',0x5354414b))})
        reports.append(result);print(json.dumps(result),flush=True)
    (a.out/'results.json').write_text(json.dumps({'pass':True,'hardware_verified':False,
        'image_sha256':hashlib.sha256((ROOT/'build/dmt99-doom.bin').read_bytes()).hexdigest(),
        'scope':'actual RV32 CPU; read-only action/motion/tick/frame observers replay the native oracle without replacing target instructions',
        'cases':reports},indent=2)+'\n')
if __name__=='__main__':main()
