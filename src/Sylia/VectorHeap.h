// Sylia is scripting language for the scripting/job system in VirtualDub
//
// Copyright (C) 2013 Avery Lee
//
// SPDX-License-Identifier: GPL-2.0-or-later
//

#ifndef f_SYLIA_VECTORHEAP_H
#define f_SYLIA_VECTORHEAP_H

class VectorHeapHeader {
public:
	VectorHeapHeader *next;
	long lSize, lPoint;
	char heap[1];
};

class VectorHeap {
private:
	VectorHeapHeader *first, *last;
	long lChunkSize;

public:
	VectorHeap(long chunk_size);
	~VectorHeap();

	void *Allocate(long);
};

#endif
