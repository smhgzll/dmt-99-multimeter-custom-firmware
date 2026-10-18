"""Read-only SCSI command oracle, NOT recovered DMT99 firmware.

Transport mock checks CBW/CSW structure for future captured-device comparisons.
No physical USB API and no image write interface are provided.
"""
import struct
class ReadOnlySCSI:
    def __init__(self,image,block_size=4096):
        if len(image)%block_size:raise ValueError('unaligned media')
        self.image=bytes(image);self.block_size=block_size;self.sense=(0,0,0)
    def command(self,cdb,lun=0):
        def fail(key,asc,ascq=0):self.sense=(key,asc,ascq);return b'',1
        if lun:return fail(5,0x25)
        if not cdb:return fail(5,0x24)
        op=cdb[0];n={0:6,3:6,0x12:6,0x1b:6,0x1e:6,0x25:10,0x28:10,0x2a:10,0x35:10}.get(op)
        if n is None:return fail(5,0x20)
        if len(cdb)!=n:return fail(5,0x24)
        if op==3:
            k,a,q=self.sense;self.sense=(0,0,0);b=bytearray(18);b[0]=0x70;b[2]=k;b[7]=10;b[12]=a;b[13]=q;return bytes(b[:cdb[4]]),0
        if op==0x12:
            if cdb[1]&1:return fail(5,0x24)
            b=bytearray(36);b[:5]=bytes([0,0x80,4,2,31]);b[8:16]=b'OFFLINE ';b[16:32]=b'REFERENCE MEDIA ';b[32:36]=b'0001';return bytes(b[:cdb[4]]),0
        if op==0x25:
            if cdb[8]&1 or any(cdb[2:6]):return fail(5,0x24)
            return struct.pack('>II',len(self.image)//self.block_size-1,self.block_size),0
        if op==0x28:
            lba=struct.unpack_from('>I',cdb,2)[0];count=struct.unpack_from('>H',cdb,7)[0]
            if lba>len(self.image)//self.block_size or count>len(self.image)//self.block_size-lba:return fail(5,0x21)
            return self.image[lba*self.block_size:(lba+count)*self.block_size],0
        if op==0x2a:return fail(7,0x27)
        return b'',0 # readiness, no-op sync/removal/start in this read-only oracle
    def bulk_in(self,cbw):
        if len(cbw)!=31:raise ValueError('CBW must be31 bytes')
        sig,tag,length,flags,lun,n=struct.unpack_from('<IIIBBB',cbw)
        if sig!=0x43425355 or not 1<=n<=16 or flags&127 or lun&240 or cbw[14]&224:raise ValueError('invalid CBW; reset recovery required')
        cdb=cbw[15:15+n];data,status=self.command(cdb,lun)
        if data and not flags&128:status=2;data=b''
        if len(data)>length:status=2;data=data[:length]
        residue=length-len(data)
        return data,struct.pack('<IIIB',0x53425355,tag,residue,status)
