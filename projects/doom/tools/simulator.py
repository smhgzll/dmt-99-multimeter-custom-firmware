#!/usr/bin/env python3
# SPDX-License-Identifier: AGPL-3.0-or-later
"""Same C application/rendering as firmware, in a desktop window or PNG."""
import argparse,ctypes,struct,zlib,time
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]

def png(rgb,w=320,h=240):
    def chunk(name,data):
        return struct.pack('>I',len(data))+name+data+struct.pack('>I',zlib.crc32(name+data))
    raw=b''.join(b'\0'+rgb[y*w*3:(y+1)*w*3] for y in range(h))
    return b'\x89PNG\r\n\x1a\n'+chunk(b'IHDR',struct.pack('>IIBBBBB',w,h,8,2,0,0,0))+chunk(b'IDAT',zlib.compress(raw,9))+chunk(b'IEND',b'')

def load():
    lib=ctypes.CDLL(str(ROOT/'build/libdoom-host.so'))
    lib.doom_host_input.argtypes=[ctypes.c_char]
    lib.doom_host_advance.argtypes=[ctypes.c_uint32]
    lib.doom_host_buttons.argtypes=[ctypes.c_uint8]
    lib.doom_host_pixels.restype=ctypes.POINTER(ctypes.c_uint16)
    lib.doom_host_init();return lib

def pixels(lib):
    p=lib.doom_host_pixels()
    return bytes(v for i in range(320*240) for v in ((p[i]>>11)*255//31,((p[i]>>5)&63)*255//63,(p[i]&31)*255//31))

def main():
    ap=argparse.ArgumentParser();ap.add_argument('--script',default='');ap.add_argument('--png',type=Path)
    a=ap.parse_args();lib=load()
    for key in a.script:
        if key=='.':lib.doom_host_advance(100)
        else:lib.doom_host_input(key.encode('ascii'))
    if a.png:a.png.write_bytes(png(pixels(lib)));print(a.png);return
    import tkinter as tk
    import base64
    win=tk.Tk();win.title('POTAT-99 DOOM — hold WASD/arrows; Space+Down 2s menu; Esc; F12 screenshot')
    label=tk.Label(win);label.pack()
    def draw():
        im=tk.PhotoImage(data=base64.b64encode(png(pixels(lib)))).zoom(2,2)
        label.configure(image=im);label.image=im
    held=set();releases={}
    mapping={'Up':2,'w':2,'Down':1,'s':1,'Left':8,'a':8,'Right':4,'d':4,'space':16,'Return':16}
    def buttons():
        mask=0
        for name in held:mask|=mapping.get(name,0)
        lib.doom_host_buttons(mask)
    def key(ev):
        if ev.keysym in releases:win.after_cancel(releases.pop(ev.keysym))
        if ev.keysym=='F12':
            out=ROOT/'build/screenshot.png';out.write_bytes(png(pixels(lib)));print(out)
        elif ev.keysym=='Escape':lib.doom_host_input(b'p');draw()
        elif ev.keysym in mapping:held.add(ev.keysym);buttons()
    def release(ev):
        def done():
            releases.pop(ev.keysym,None);held.discard(ev.keysym);buttons()
        releases[ev.keysym]=win.after_idle(done) # cancel synthetic autorepeat releases
    def unfocus(ev):
        held.clear();buttons()
    previous=time.monotonic()
    def update():
        nonlocal previous
        now=time.monotonic();elapsed=min(500,int((now-previous)*1000));previous=now
        lib.doom_host_advance(elapsed);draw();win.after(20,update)
    win.bind('<KeyPress>',key);win.bind('<KeyRelease>',release);win.bind('<FocusOut>',unfocus)
    draw();win.after(20,update);win.mainloop()
if __name__=='__main__':main()
