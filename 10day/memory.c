/* �������֌W */

#include "bootpack.h"

#define EFLAGS_AC_BIT		0x00040000
#define CR0_CACHE_DISABLE	0x60000000

unsigned int memtest(unsigned int start, unsigned int end)
{
	char flg486 = 0;
	unsigned int eflg, cr0, i;

	/* 386���A486�ȍ~�Ȃ̂��̊m�F */
	eflg = io_load_eflags();
	eflg |= EFLAGS_AC_BIT; /* AC-bit = 1 */
	io_store_eflags(eflg);
	eflg = io_load_eflags();
	if ((eflg & EFLAGS_AC_BIT) != 0) { /* 386�ł�AC=1�ɂ��Ă�������0�ɖ߂��Ă��܂� */
		flg486 = 1;
	}
	eflg &= ~EFLAGS_AC_BIT; /* AC-bit = 0 */
	io_store_eflags(eflg);

	if (flg486 != 0) {
		cr0 = load_cr0();
		cr0 |= CR0_CACHE_DISABLE; /* �L���b�V���֎~ */
		store_cr0(cr0);
	}

	i = memtest_sub(start, end);

	if (flg486 != 0) {
		cr0 = load_cr0();
		cr0 &= ~CR0_CACHE_DISABLE; /* �L���b�V������ */
		store_cr0(cr0);
	}

	return i;
}

void memman_init(struct MEMMAN *man)
{
	man->frees = 0;			/* �������̌� */
	man->maxfrees = 0;		/* �󋵊ώ@�p�Ffrees�̍ő�l */
	man->lostsize = 0;		/* ����Ɏ��s�������v�T�C�Y */
	man->losts = 0;			/* ����Ɏ��s������ */
	return;
}

unsigned int memman_total(struct MEMMAN *man)
/* �����T�C�Y�̍��v��� */
{
	unsigned int i, t = 0;
	for (i = 0; i < man->frees; i++) {
		t += man->free[i].size;
	}
	return t;
}

unsigned int memman_alloc(struct MEMMAN *man, unsigned int size)
/* �m�� */
{
	unsigned int i, a;
	for (i = 0; i < man->frees; i++) {
		if (man->free[i].size >= size) {
			/* �\���ȍL���̂����𔭌� */
			a = man->free[i].addr;
			man->free[i].addr += size;
			man->free[i].size -= size;
			if (man->free[i].size == 0) {
				/* free[i]���Ȃ��Ȃ����̂őO�ւ߂� */
				man->frees--;
				for (; i < man->frees; i++) {
					man->free[i] = man->free[i + 1]; /* �\���̂̑�� */
				}
			}
			return a;
		}
	}
	return 0; /* �������Ȃ� */
}

int memman_free(struct MEMMAN *man, unsigned int addr, unsigned int size)
/* ��� */
{
	int i, j;
	/* �܂Ƃ߂₷�����l����ƁAfree[]��addr���ɕ���ł���ق������� */
	/* ������܂��A�ǂ��ɓ����ׂ��������߂� */
	for (i = 0; i < man->frees; i++) {
		if (man->free[i].addr > addr) {
			break;
		}
	}
	/* free[i - 1].addr < addr < free[i].addr */
	if (i > 0) {
		/* �O������ */
		if (man->free[i - 1].addr + man->free[i - 1].size == addr) {
			/* �O�̂����̈�ɂ܂Ƃ߂��� */
			man->free[i - 1].size += size;
			if (i < man->frees) {
				/* �������� */
				if (addr + size == man->free[i].addr) {
					/* �Ȃ�ƌ��Ƃ��܂Ƃ߂��� */
					man->free[i - 1].size += man->free[i].size;
					/* man->free[i]�̍폜 */
					/* free[i]���Ȃ��Ȃ����̂őO�ւ߂� */
					man->frees--;
					for (; i < man->frees; i++) {
						man->free[i] = man->free[i + 1]; /* �\���̂̑�� */
					}
				}
			}
			return 0; /* �����I�� */
		}
	}
	/* �O�Ƃ͂܂Ƃ߂��Ȃ����� */
	if (i < man->frees) {
		/* ��낪���� */
		if (addr + size == man->free[i].addr) {
			/* ���Ƃ͂܂Ƃ߂��� */
			man->free[i].addr = addr;
			man->free[i].size += size;
			return 0; /* �����I�� */
		}
	}
	/* �O�ɂ����ɂ��܂Ƃ߂��Ȃ� */
	if (man->frees < MEMMAN_FREES) {
		/* free[i]�������A���ւ��炵�āA�����܂���� */
		for (j = man->frees; j > i; j--) {
			man->free[j] = man->free[j - 1];
		}
		man->frees++;
		if (man->maxfrees < man->frees) {
			man->maxfrees = man->frees; /* �ő�l���X�V */
		}
		man->free[i].addr = addr;
		man->free[i].size = size;
		return 0; /* �����I�� */
	}
	/* ���ɂ��点�Ȃ����� */
	man->losts++;
	man->lostsize += size;
	return -1; /* ���s�I�� */
}

unsigned int memman_alloc_4k(struct MEMMAN *man, unsigned int size)
{
	unsigned int a;
	size = (size + 0xfff) & 0xfffff000;
	a = memman_alloc(man, size);
	return a;
}

int memman_free_4k(struct MEMMAN *man, unsigned int addr, unsigned int size){
	int i;
	size = (size + 0xfff) & 0xfffff000;
	i = memman_free(man, addr, size);
	return i;
}

void memcpy(void *dst, void *src, unsigned long long size){
	unsigned char *d = (unsigned char *)dst;
	unsigned char *s = (unsigned char *)src;
	for (; size > 0; size--)
		*d++ = *s++;
}