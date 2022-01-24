#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdint.h>
#include <string.h>

struct fs_header {
  char romf[8];
  uint32_t fullsize;
  uint32_t checksum;
  char name[];
} ;

uint32_t fs_to_native(uint32_t x) {
  uint8_t *cast = (uint8_t*)&x;
  return (cast[0]<<24) | (cast[1]<<16) | (cast[2]<<8) | cast[3];
}

void decode(struct fs_header *p, size_t size){
  assert(strncmp(p->romf,"-rom1fs-",sizeof(p->romf))==0);
  uint32_t unzip_size = fs_to_native(p->fullsize);
  assert((unzip_size < size));
};

int main(void){

  int fd = open("tp1fs.romfs",O_RDONLY);
  assert(fd != -1);
  off_t fsize;
  fsize = lseek(fd,0,SEEK_END);

  //  printf("size is %d", fsize);
  
  void *addr = mmap(NULL, fsize, PROT_READ, MAP_SHARED, fd, 0);
  assert(addr != MAP_FAILED);
  decode((struct fs_header *)addr, fsize);
  return 0;
}
