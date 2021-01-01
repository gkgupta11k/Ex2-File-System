#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <iomanip>
#include<cstring>


void displayBuffer (uint8_t *,int);
void displayBufferPage (uint8_t *, uint32_t, uint32_t, uint64_t);
void step0(struct VDIFile *);
void partitionData(struct partitionEntry[]);

struct __attribute__((packed))HeaderStructure{

    char szFileInfo[64];
    uint32_t u32Signature;
    uint32_t u32Version;
    uint32_t cbHeader;
    uint32_t u32Type;
    uint32_t fFlags;
    uint8_t szComment[256];
    uint32_t offBlocks;
    uint32_t offData;
    uint32_t cCylinders;
    uint32_t cHeads;
    uint32_t cSectors;
    uint32_t cbSector;
    uint32_t u32Dummy;
    uint64_t cbDisk;
    uint32_t cbBlock;
    uint32_t cbBlockExtra;
    uint32_t cBlocks;
    uint32_t cBlocksAllocated;
    char uuidCreate[16];
    char uuidModify[16];
    char uuidLinkage[16];
    char uuidParentModify[16];
};


struct __attribute__((packed))partitionEntry {
  unsigned char status;
  unsigned char firstSectorCHS[3];
  unsigned char partitionType;
  uint8_t lastSectorCHS[3];
  uint32_t firstLBASector;
  uint32_t sectorCount;
};

struct __attribute__((packed))superBlock {
  uint32_t	s_inodes_count;
  uint32_t	s_blocks_count;
  uint32_t	s_r_blocks_count;
  uint32_t	s_free_blocks_count;
  uint32_t	s_free_inodes_count;
  uint32_t	s_first_data_block;
  uint32_t	s_log_block_size;
  uint32_t	s_log_frag_size;
  uint32_t	s_blocks_per_group;
  uint32_t	s_frags_per_group;
  uint32_t	s_inodes_per_group;
  uint32_t	s_mtime;
  uint32_t	s_wtime;
  uint16_t	s_mnt_count;
  uint16_t	s_max_mnt_count;
  uint16_t	s_magic;
  uint16_t	s_state;
  uint16_t	s_errors;
  uint16_t	s_minor_rev_level;
  uint32_t	s_lastcheck;
  uint32_t	s_checkinterval;
  uint32_t	s_creator_os;
  uint32_t	s_rev_level;
  uint16_t	s_def_resuid;
  uint16_t	s_def_resgid;

  uint32_t	s_first_ino;
  uint16_t	s_inode_size;
  uint16_t	s_block_group_nr;
  uint32_t	s_feature_compat;
  uint32_t	s_feature_incompat;
  uint32_t	s_feature_ro_compat;
  uint32_t	s_uuid[4];
  uint32_t	s_volume_name[4];
  uint32_t	s_last_mounted[16];
  uint32_t	s_algo_bitmap;

  uint8_t	s_prealloc_blocks;
  uint8_t	s_prealloc_dir_blocks;
  uint16_t	alignment;

  uint32_t	s_journal_uuid[4];
  uint32_t	s_journal_inum;
  uint32_t	s_journal_dev;
  uint32_t	s_last_orphan;

  uint32_t	s_hash_seed[4];
  uint8_t	s_def_hash_version;
  uint8_t	padding[3];

  uint32_t	s_default_mount_options;
  uint32_t	s_first_meta_bg;
};
struct  __attribute__((packed)) blockGroupDescriptor {
	
};

struct __attribute__((packed))VDIFile {
    int fd;
    HeaderStructure header;
    ssize_t cursor;
};
struct VDIFile *vdiOpen (char *fn);
void vdiClose (struct VDIFile *f);
ssize_t vdiRead (struct VDIFile *f,void *buf,size_t count);
ssize_t  vdiWrite (struct VDIFile *f,void *buf,size_t count);
off_t vdiSeek (VDIFile *f,off_t offset, int anchor);
void dumpVDIHeader (struct VDIFile *);


using namespace std;
int main(int argc, char *argv[]){

    struct VDIFile *f = vdiOpen(argv[1]);
    step0(f);
    dumpVDIHeader(f);
    cout<<endl;
    cout<<endl;
    uint8_t buffer[400];
    lseek(f->fd, 0, SEEK_SET);
    vdiRead(f,buffer,sizeof(buffer));
    displayBuffer(buffer,sizeof(buffer));
    cout<<endl;
    cout<<endl;
    struct partitionEntry entries[4];
    vdiSeek(f,446,SEEK_SET);
    vdiRead(f,&entries,sizeof(entries));
    partitionData(entries);
    struct superBlock*super=(struct superBlock*)malloc(sizeof(entries));
    vdiSeek(f,entries[0].firstLBASector*512+1024,SEEK_SET);
    vdiRead(f,super,sizeof(struct superBlock));
    cout<<endl<<endl<<"superblock data "<<super->s_inodes_count<<endl;
    free(f);
    free(super);
}

void step0(struct VDIFile *f){
  char buf[39];
  uint8_t buffer[400];

  read (f->fd,buf,sizeof(buf));
  lseek(f->fd,0,SEEK_SET);
  int count=0;

  int totalNumberOfBytesRead = read(f->fd, buffer, sizeof(buffer));
  for (uint8_t x: buffer){
    cout<< std::hex<<(int)x<<"\t";
    count++;
    if (count==16){
      cout<<endl;
      count=0;
    }
  }

  cout<<"Total bytes read:: "<<std::dec<<totalNumberOfBytesRead<<endl;

}

void partitionData(struct partitionEntry entries[]){


  for(int j=0; j<4; j++){
  cout<< "Status: "<<(int)entries[j].status<<endl;
  cout<< "First Sector CHS :  ";
  for(int i=0; i<3; i++){
  cout<< (int)entries[j].firstSectorCHS[i]<<"-";

  }
  cout<<endl;
  cout<< "Partition Type:  " <<std::hex<<(int) entries[j].partitionType<<endl;
  cout<< "Last Sector CHS:  " ;
  for(int i=0; i<3; i++){
  cout<< (int)entries[j].lastSectorCHS[i]<<"-";

  }
  cout<<endl;
  cout<< "First LBA Sector:  " << std::dec<< entries[j].firstLBASector<<endl;
  cout<< "Sector Count:  " << std::dec<< entries[j].sectorCount<<endl;
  cout<<endl;
  cout<<endl;
  }
}
void displayBufferPage(uint8_t *buf, uint32_t count, uint32_t start,uint64_t offset) {
    cout << "Printing buffer in hexadecimal" << endl;
    cout << endl;
    int counter = 0;
    for (int i = start ; i <=  start+count; i++) {
        if (start <= i && i < start + count) {
            uint8_t byte = buf[i];
            cout << setfill('0') << setw(2) << hex << (int) (byte) << " ";
        } else {
            cout <<endl;
            break;
        }

        counter++;
        if (counter % 16 ==0){
            cout << endl;
        }

    }

    cout << endl;
    cout << "Printing buffer in text" << endl;
    cout << endl;

    for (int i = start; i <= count; i++) {
        if (start <= i && i < start + count) {
            uint8_t byte = buf[i];
            if (isprint(int(byte))) {
                cout << byte << " ";
            } else {
                cout << " ";
            }
        }
        else{
            cout << endl;
            break;
        }
    }
}

void displayBuffer(uint8_t *buf, int size){
  int count = 0;
  for (int i=0; i<size; i++){
    cout<< std::hex<<(int)*(buf+i)<<"\t";
    count++;
    if (count==16){
      cout<<endl;
      count=0;
    }
  }

}

struct VDIFile *vdiOpen (char *fn){
    int fd = open(fn, O_RDWR);

    if (fd !=-1){
      struct VDIFile * file = (struct VDIFile *) malloc(sizeof(struct VDIFile));
      read(fd,&file->header,sizeof(file->header));
      file->cursor= file->header.offData;
      file->fd=fd;
      return file;
    }

    else {

        return nullptr;
    }
}

void vdiClose (struct VDIFile *f){

    close((*f).fd);

    delete f;
}

ssize_t vdiRead (struct VDIFile *f,void *buf,size_t count){
    int readBytes= read(f->fd,buf, count);
    if(readBytes==-1){
      cout<<"Unable to read the vdi file "<<"\n";
    }

    return readBytes;
}

ssize_t  vdiWrite (struct VDIFile *f,void *buf, size_t count){
    lseek((*f).fd, (*f).cursor+ (*f).header.offData, SEEK_SET);
    int numofBytesRead = write( (*f).fd,buf,count);
    return 0;
}

off_t vdiSeek (VDIFile *f, off_t offset, int anchor){
  if(anchor==SEEK_SET && (f->cursor+offset)<f->header.cbDisk){
    return lseek(f->fd,f->cursor + offset,SEEK_SET);
  }
  else if(anchor == SEEK_CUR && (f->cursor+offset)<f->header.cbDisk){
    return lseek(f->fd,f->cursor + offset,SEEK_SET);
  }
  else if(anchor ==SEEK_END && offset<=0){
    return lseek(f->fd,f->cursor + offset,SEEK_SET);

  }
}

int32_t fetchBlockFromFile(struct Inode *i,uint32_t bNum,
void *buf){
    k ← block size /4
 if b < 12 then 
 blockList ← i_block

 goto direct
 else if b < 12 + k then 
 if i_block[12] = 0 then
 return false
 end if
 FetchBlock(i_block[12], buƒ ) 
 blockList ← buƒ 
 b ← b − 12 

 goto direct
 else if b < 12 + k + k^2 //square 
if i_block[13] = 0 then
 return false
 end if
FetchBlock(i_block[13], buƒ ) 
 blockList ← buƒ 
b ← b − 12 − k 
 goto single
 else 
 if i_block[14] = 0 then
 return false
 end if
 FetchBlock(i_block[14], buƒ ) 
blockList ← buƒ 
 b ← b − 12 − k − k^2 //square
 end if

 index ← b/ (k^2) 
  b ← b mod (k^2)
   if blockList[index] = 0 then
 return false
 end if
FetchBlock(blockList[index], buƒ )
 blockList ← buƒ

single: 
 index ← b/ k 
 b ← b mod k 
 if blockList[index] = 0 then
 return false
 end if
 FetchBlock(blockList[index], buƒ) 
 blockList ← buƒ

if blockList[b] = 0 then
return false
 end if
 FetchBlock(blockList[b], buƒ ) 
 return true
}

int32_t writeBlockToFile(struct Inode *i,uint32_t bNum,
void *buf){
	k ← block size /4
 if b < 12 then 
 if i_block[b] == 0 then
 i_block[b] ← Allocate()
  WriteInode(iNum, iNode)
 end if
blockList ← i_block 
 goto direct
else if b < 12 + k then 
 if i_block[12] == 0 then
 i_block[12] ← Allocate()
 WriteInode(iNum, iNode)
end if
 FetchBlock(i_block[12], tmp) 

 ibNum ← i_block[12]
 blockList ← tmp 
 b ← b − 12 

 goto direct
 else if b < 12 + k + k^2 then
if i_block[13] = 0 then
i_block[13] ← Allocate()
 WriteInode(iNum, iNode)
 end if
 FetchBlock(i_block[13], tmp) 

 ibNum ← i_block[13]
 blockList ← tmp 
 b ← b − 12 − k 

 goto single
 else 
 if i_block[14] = 0 then
 i_block[14] ← Allocate()
 WriteInode(iNum, iNode)
 end if
FetchBlock(i_block[14], tmp) 
 ibNum ← i_block[14]
 blockList ← tmp 
 b ← b − 12 − k − k^2

 end if

  index ← b/ (k^2) 
 b ← b mod (k^2) 

  if blockList[index] = 0 then
blockList[index] ← Allocate()
WriteBlock(ibNum, blockList)
 end if
ibNum ← blockList[index]
 FetchBlock(blockList[index], tmp) 
 blockList ← tmp

 single:

 index ← b/ k 
 b ← b mod k 

 if blockList[index] = 0 then
 blockList[Index] ← Allocate()
 WriteBlock(ibNum, blockList)
 end if
 ibNum ← blockList[index]
 FetchBlock(blockList[index], tmp) 
 blockList ← tmp

 direct: 

 if blockList[b] = 0 then
 blockList[b] ← Allocate()
 WriteBlock(ibNum, blockList)
 end if
 WriteBlock(blockList[b], buƒ )
}

void dumpVDIHeader(struct VDIFile *f){
    cout <<  hex << (int) (f->header.u32Signature) <<endl ;
    cout << hex << (int) (f->header.u32Version) << endl;
    cout << hex << (int) (f->header.cbHeader) << endl;
    cout << hex << (int) (f->header.u32Type) << endl;
    cout << hex << (int) (f->header.offBlocks) << endl;
    cout << hex << (int) (f->header.offData) << endl;
    cout << hex << (int) (f->header.cCylinders) << endl;
    cout << hex << (int) (f->header.cHeads) << endl;
    cout << hex << (int) (f->header.cSectors) << endl;
    cout << hex << (int) (f->header.cbSector) << endl;
    cout << hex << (int) (f->header.u32Dummy) << endl;
    cout << hex << "Disk Size "<<(int) (f->header.cbDisk) << endl;
    cout << hex << (int) (f->header.cbBlock) << endl;
    cout << hex << (int) (f->header.cbBlockExtra) << endl;
    cout << hex << (int) (f->header.cBlocks) << endl;
    cout << hex << (int) (f->header.cBlocksAllocated) << endl;

    int count = 0;
    for (uint8_t x: f->header.szComment){
      cout<< std::hex<<(int)x<<"\t";
      count++;
      if (count==16){
        cout<<endl;
        count=0;
      }
    }


}
