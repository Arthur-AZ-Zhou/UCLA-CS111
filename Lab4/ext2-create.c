#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef int16_t i16;
typedef int32_t i32;

#define BLOCK_SIZE 1024
#define BLOCK_OFFSET(i) (i * BLOCK_SIZE)
#define NUM_BLOCKS 1024
#define NUM_INODES 128

#define LOST_AND_FOUND_INO 11
#define HELLO_WORLD_INO    12
#define HELLO_INO          13
#define LAST_INO           HELLO_INO

#define SUPERBLOCK_BLOCKNO             1
#define BLOCK_GROUP_DESCRIPTOR_BLOCKNO 2
#define BLOCK_BITMAP_BLOCKNO           3
#define INODE_BITMAP_BLOCKNO           4
#define INODE_TABLE_BLOCKNO            5
#define ROOT_DIR_BLOCKNO               21
#define LOST_AND_FOUND_DIR_BLOCKNO     22
#define HELLO_WORLD_FILE_BLOCKNO       23
#define LAST_BLOCK                     HELLO_WORLD_FILE_BLOCKNO

#define NUM_FREE_BLOCKS (NUM_BLOCKS - LAST_BLOCK - 1)
#define NUM_FREE_INODES (NUM_INODES - LAST_INO)

#define EXT2_SUPER_MAGIC 0xEF53

/* http://www.nongnu.org/ext2-doc/ext2.html */
/* http://www.science.smith.edu/~nhowe/262/oldlabs/ext2.html */

#define	EXT2_BAD_INO             1
#define EXT2_ROOT_INO            2
#define EXT2_GOOD_OLD_FIRST_INO 11

#define EXT2_GOOD_OLD_REV 0

#define EXT2_S_IFSOCK 0xC000
#define EXT2_S_IFLNK  0xA000
#define EXT2_S_IFREG  0x8000
#define EXT2_S_IFBLK  0x6000
#define EXT2_S_IFDIR  0x4000
#define EXT2_S_IFCHR  0x2000
#define EXT2_S_IFIFO  0x1000
#define EXT2_S_ISUID  0x0800
#define EXT2_S_ISGID  0x0400
#define EXT2_S_ISVTX  0x0200
#define EXT2_S_IRUSR  0x0100
#define EXT2_S_IWUSR  0x0080
#define EXT2_S_IXUSR  0x0040
#define EXT2_S_IRGRP  0x0020
#define EXT2_S_IWGRP  0x0010
#define EXT2_S_IXGRP  0x0008
#define EXT2_S_IROTH  0x0004
#define EXT2_S_IWOTH  0x0002
#define EXT2_S_IXOTH  0x0001

#define	EXT2_NDIR_BLOCKS 12
#define	EXT2_IND_BLOCK   EXT2_NDIR_BLOCKS
#define	EXT2_DIND_BLOCK  (EXT2_IND_BLOCK + 1)
#define	EXT2_TIND_BLOCK  (EXT2_DIND_BLOCK + 1)
#define	EXT2_N_BLOCKS    (EXT2_TIND_BLOCK + 1)

#define EXT2_NAME_LEN 255

struct ext2_superblock {
	u32 s_inodes_count;
	u32 s_blocks_count;
	u32 s_r_blocks_count;
	u32 s_free_blocks_count;
	u32 s_free_inodes_count;
	u32 s_first_data_block;
	u32 s_log_block_size;
	i32 s_log_frag_size;
	u32 s_blocks_per_group;
	u32 s_frags_per_group;
	u32 s_inodes_per_group;
	u32 s_mtime;
	u32 s_wtime;
	u16 s_mnt_count;
	i16 s_max_mnt_count;
	u16 s_magic;
	u16 s_state;
	u16 s_errors;
	u16 s_minor_rev_level;
	u32 s_lastcheck;
	u32 s_checkinterval;
	u32 s_creator_os;
	u32 s_rev_level;
	u16 s_def_resuid;
	u16 s_def_resgid;
	u32 s_pad[5];
	u8 s_uuid[16];
	u8 s_volume_name[16];
	u32 s_reserved[229];
};

struct ext2_block_group_descriptor
{
	u32 bg_block_bitmap;
	u32 bg_inode_bitmap;
	u32 bg_inode_table;
	u16 bg_free_blocks_count;
	u16 bg_free_inodes_count;
	u16 bg_used_dirs_count;
	u16 bg_pad;
	u32 bg_reserved[3];
};

struct ext2_inode {
	u16 i_mode;
	u16 i_uid;
	u32 i_size;
	u32 i_atime;
	u32 i_ctime;
	u32 i_mtime;
	u32 i_dtime;
	u16 i_gid;
	u16 i_links_count;
	u32 i_blocks;
	u32 i_flags;
	u32 i_reserved1;
	u32 i_block[EXT2_N_BLOCKS];
	u32 i_version;
	u32 i_file_acl;
	u32 i_dir_acl;
	u32 i_faddr;
	u8  i_frag;
	u8  i_fsize;
	u16 i_pad1;
	u32 i_reserved2[2];
};

struct ext2_dir_entry {
	u32 inode;
	u16 rec_len;
	u16 name_len;
	u8  name[EXT2_NAME_LEN];
};

#define errno_exit(str)                                                        \
	do { int err = errno; perror(str); exit(err); } while (0)

#define dir_entry_set(entry, inode_num, str)                                   \
	do {                                                                   \
		char *s = str;                                                 \
		size_t len = strlen(s);                                        \
		entry.inode = inode_num;                                       \
		entry.name_len = len;                                          \
		memcpy(&entry.name, s, len);                                   \
		if ((len % 4) != 0) {                                          \
			entry.rec_len = 12 + len / 4 * 4;                      \
		}                                                              \
		else {                                                         \
			entry.rec_len = 8 + len;                               \
		}                                                              \
	} while (0)

#define dir_entry_write(entry, fd)                                             \
	do {                                                                   \
		size_t size = entry.rec_len;                                   \
		if (write(fd, &entry, size) != size) {                         \
			errno_exit("write");                                   \
		}                                                              \
	} while (0)

u32 get_current_time() {
	time_t t = time(NULL);
	if (t == ((time_t) -1)) {
		errno_exit("time");
	}
	return t;
}

void write_superblock(int fd) {
	off_t off = lseek(fd, BLOCK_OFFSET(1), SEEK_SET);
	if (off == -1) {
		errno_exit("lseek");
	}

	u32 current_time = get_current_time();

	struct ext2_superblock superblock = {0};

	// TODO It's all yours
	// TODO finish the superblock number setting
	superblock.s_inodes_count = -1;
	superblock.s_blocks_count = -1;
	superblock.s_r_blocks_count = 0;
	superblock.s_free_blocks_count = -1;
	superblock.s_free_inodes_count = -1;
	superblock.s_first_data_block = -1; /* First Data Block */
	superblock.s_log_block_size = 0;					/* 1024 */
	superblock.s_log_frag_size = 0;						/* 1024 */
	superblock.s_blocks_per_group = -1;
	superblock.s_frags_per_group = -1;
	superblock.s_inodes_per_group = -1;
	superblock.s_mtime = 0;				/* Mount time */
	superblock.s_wtime = current_time;	/* Write time */
	superblock.s_mnt_count         = 0; /* Number of times mounted so far */
	superblock.s_max_mnt_count     = 0; /* Make this unlimited */
	superblock.s_magic = -1; /* ext2 Signature */
	superblock.s_state             = 0; /* File system is clean */
	superblock.s_errors            = 0; /* Ignore the error (continue on) */
	superblock.s_minor_rev_level   = 0; /* Leave this as 0 */
	superblock.s_lastcheck = current_time; /* Last check time */
	superblock.s_checkinterval     = 0; /* Force checks by making them every 1 second */
	superblock.s_creator_os        = 0; /* Linux */
	superblock.s_rev_level         = 0; /* Leave this as 0 */
	superblock.s_def_resuid        = 0; /* root */
	superblock.s_def_resgid        = 0; /* root */

	/* You can leave everything below this line the same, delete this
	   comment when you're done the lab */
	superblock.s_uuid[0] = 0x5A;
	superblock.s_uuid[1] = 0x1E;
	superblock.s_uuid[2] = 0xAB;
	superblock.s_uuid[3] = 0x1E;
	superblock.s_uuid[4] = 0x13;
	superblock.s_uuid[5] = 0x37;
	superblock.s_uuid[6] = 0x13;
	superblock.s_uuid[7] = 0x37;
	superblock.s_uuid[8] = 0x13;
	superblock.s_uuid[9] = 0x37;
	superblock.s_uuid[10] = 0xC0;
	superblock.s_uuid[11] = 0xFF;
	superblock.s_uuid[12] = 0xEE;
	superblock.s_uuid[13] = 0xC0;
	superblock.s_uuid[14] = 0xFF;
	superblock.s_uuid[15] = 0xEE;

	memcpy(&superblock.s_volume_name, "cs111-base", 10);

	ssize_t size = sizeof(superblock);
	if (write(fd, &superblock, size) != size) {
		errno_exit("write");
	}
}

void write_block_group_descriptor_table(int fd) {
	off_t off = lseek(fd, BLOCK_OFFSET(BLOCK_GROUP_DESCRIPTOR_BLOCKNO), SEEK_SET);
	if (off == -1) {
		errno_exit("lseek");
	}

	struct ext2_block_group_descriptor block_group_descriptor = {0};

	// TODO It's all yours
	// TODO finish the block group descriptor number setting
	block_group_descriptor.bg_block_bitmap = -1;
	block_group_descriptor.bg_inode_bitmap = -1;
	block_group_descriptor.bg_inode_table = -1;
	block_group_descriptor.bg_free_blocks_count = -1;
	block_group_descriptor.bg_free_inodes_count = -1;
	block_group_descriptor.bg_used_dirs_count = -1;

	ssize_t size = sizeof(block_group_descriptor);
	if (write(fd, &block_group_descriptor, size) != size) {
		errno_exit("write");
	}
}

void write_block_bitmap(int fd)
{
	off_t off = lseek(fd, BLOCK_OFFSET(BLOCK_BITMAP_BLOCKNO), SEEK_SET);
	if (off == -1)
	{
		errno_exit("lseek");
	}

	// TODO It's all yours
	u8 map_value[BLOCK_SIZE];

	if (write(fd, map_value, BLOCK_SIZE) != BLOCK_SIZE)
	{
		errno_exit("write");
	}
}

void write_inode_bitmap(int fd)
{
	off_t off = lseek(fd, BLOCK_OFFSET(INODE_BITMAP_BLOCKNO), SEEK_SET);
	if (off == -1)
	{
		errno_exit("lseek");
	}

	// TODO It's all yours
	u8 map_value[BLOCK_SIZE];

	if (write(fd, map_value, BLOCK_SIZE) != BLOCK_SIZE)
	{
		errno_exit("write");
	}
}

void write_inode(int fd, u32 index, struct ext2_inode *inode) {
	off_t off = BLOCK_OFFSET(INODE_TABLE_BLOCKNO)
	            + (index - 1) * sizeof(struct ext2_inode);
	off = lseek(fd, off, SEEK_SET);
	if (off == -1) {
		errno_exit("lseek");
	}

	ssize_t size = sizeof(struct ext2_inode);
	if (write(fd, inode, size) != size) {
		errno_exit("write");
	}
}

void write_inode_table(int fd) {
	u32 current_time = get_current_time();

	struct ext2_inode lost_and_found_inode = {0};
	lost_and_found_inode.i_mode = EXT2_S_IFDIR
	                              | EXT2_S_IRUSR
	                              | EXT2_S_IWUSR
	                              | EXT2_S_IXUSR
	                              | EXT2_S_IRGRP
	                              | EXT2_S_IXGRP
	                              | EXT2_S_IROTH
	                              | EXT2_S_IXOTH;
	lost_and_found_inode.i_uid = 0;
	lost_and_found_inode.i_size = 1024;
	lost_and_found_inode.i_atime = current_time;
	lost_and_found_inode.i_ctime = current_time;
	lost_and_found_inode.i_mtime = current_time;
	lost_and_found_inode.i_dtime = 0;
	lost_and_found_inode.i_gid = 0;
	lost_and_found_inode.i_links_count = 2;
	lost_and_found_inode.i_blocks = 2; /* These are oddly 512 blocks */
	lost_and_found_inode.i_block[0] = LOST_AND_FOUND_DIR_BLOCKNO;
	write_inode(fd, LOST_AND_FOUND_INO, &lost_and_found_inode);

	// TODO It's all yours
	// TODO finish the inode entries for the other files
}

void write_root_dir_block(int fd)
{
	// TODO It's all yours
}

void write_lost_and_found_dir_block(int fd) {
	off_t off = BLOCK_OFFSET(LOST_AND_FOUND_DIR_BLOCKNO);
	off = lseek(fd, off, SEEK_SET);
	if (off == -1) {
		errno_exit("lseek");
	}

	ssize_t bytes_remaining = BLOCK_SIZE;

	struct ext2_dir_entry current_entry = {0};
	dir_entry_set(current_entry, LOST_AND_FOUND_INO, ".");
	dir_entry_write(current_entry, fd);

	bytes_remaining -= current_entry.rec_len;

	struct ext2_dir_entry parent_entry = {0};
	dir_entry_set(parent_entry, EXT2_ROOT_INO, "..");
	dir_entry_write(parent_entry, fd);

	bytes_remaining -= parent_entry.rec_len;

	struct ext2_dir_entry fill_entry = {0};
	fill_entry.rec_len = bytes_remaining;
	dir_entry_write(fill_entry, fd);
}

void write_hello_world_file_block(int fd)
{
	// TODO It's all yours
}

int main(int argc, char *argv[]) {
	int fd = open("cs111-base.img", O_CREAT | O_WRONLY, 0666);
	if (fd == -1) {
		errno_exit("open");
	}

	if (ftruncate(fd, 0)) {
		errno_exit("ftruncate");
	}
	if (ftruncate(fd, NUM_BLOCKS * BLOCK_SIZE)) {
		errno_exit("ftruncate");
	}

	write_superblock(fd);
	write_block_group_descriptor_table(fd);
	write_block_bitmap(fd);
	write_inode_bitmap(fd);
	write_inode_table(fd);
	write_root_dir_block(fd);
	write_lost_and_found_dir_block(fd);
	write_hello_world_file_block(fd);

	if (close(fd)) {
		errno_exit("close");
	}
	return 0;
}
