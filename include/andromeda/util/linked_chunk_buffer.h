#ifndef ANDROMEDA_UTIL_LINKEDCHUNKBUFFER
#define ANDROMEDA_UTIL_LINKEDCHUNKBUFFER

#include <iostream>
#include <malloc.h>
#include <string.h>

namespace andromeda {
	namespace util {
		template<typename E>
		class LinkedChunkBuffer
		{
		protected:
			class Chunk
			{
			public:
				Chunk* last;
				E* data;
				Chunk* next;
				Chunk(LinkedChunkBuffer *buffer,Chunk *last_chunk,Chunk *next_chunk) :
						last(last_chunk), next(next_chunk), data((E*)malloc(buffer->chunk_size*sizeof(E)))
				{
				}

				~Chunk()
				{
					if(this)
					{
						if(last!=nullptr)
							last->next=nullptr;
						free(data);
						delete next;
					}
				}
			};
		public:
			Chunk* first_chunk;
			Chunk* last_chunk;
			Chunk* last_chunk_for_pos;

			Chunk* getChunkByIndex(int index)
			{
				Chunk* chunk=first_chunk;
				for(int i=0;i<index/chunk_size;++i)
					chunk=chunk->next;
				return chunk;
			}

			Chunk* getChunkByRelativeIndex(Chunk *fid_chunk,int fid_index,int index)
			{
				Chunk* chunk=fid_chunk;
				if(index>0)
				{
					int chunk_length=(fid_index+index)/chunk_size;
					for(int i=0;i<chunk_length;++i)
						chunk=chunk->next;
				}
				else if(index<0)
				{
					int tmp=fid_index+index;
					if(tmp<0)
					{
						int chunk_length=(-tmp)/chunk_size;
						for(int i=0;i<chunk_length;++i)
							chunk=chunk->last;
					}
				}
				return chunk;
			}

		public:
			int chunk_size;
			int chunk_count;
			int data_size;

			LinkedChunkBuffer(int size_of_chunk)
			{
				reset(size_of_chunk);
			}

			LinkedChunkBuffer(const LinkedChunkBuffer &buffer)
			{
				chunk_size=buffer.chunk_size;
				chunk_count=buffer.chunk_count;
				data_size=buffer.data_size;
				first_chunk=buffer.first_chunk;
				last_chunk=buffer.last_chunk;
				last_chunk_for_pos=buffer.last_chunk_for_pos;
			}

			LinkedChunkBuffer(LinkedChunkBuffer &&buffer)
			{
				chunk_size=buffer.chunk_size;
				chunk_count=buffer.chunk_count;
				data_size=buffer.data_size;
				first_chunk=buffer.first_chunk;
				last_chunk=buffer.last_chunk;
				last_chunk_for_pos=buffer.last_chunk_for_pos;
				buffer.first_chunk=nullptr;
			}

			~LinkedChunkBuffer()
			{
				delete first_chunk;
			}

			void reset(int size_of_chunk)
			{
				first_chunk=nullptr;
				last_chunk=nullptr;
				chunk_count=0;
				data_size=0;
				chunk_size=size_of_chunk;
				last_chunk_for_pos=nullptr;
			}

			void alloc_chunk(int num)
			{
				for(int i=0;i<num;++i)
				{
					Chunk* new_chunk=new Chunk(this,last_chunk,nullptr);
					++chunk_count;
					if(first_chunk==nullptr)
					{
						first_chunk=new_chunk;
						last_chunk=new_chunk;
						continue;
					}
					last_chunk->next=new_chunk;
					last_chunk=new_chunk;
				}
			}

			void put(E *elements,int length)
			{
				if(length<1)
					return;
				for(int i=0;i<length;++i)
				{
					int relative_pos=data_size%chunk_size;
					if(relative_pos==0)
					{
						if(last_chunk_for_pos==last_chunk)
							alloc_chunk(1);
						if(data_size==0)
							last_chunk_for_pos=first_chunk;
						else
							last_chunk_for_pos=last_chunk_for_pos->next;
					}
					*(last_chunk_for_pos->data+relative_pos)=*(elements+i);
					++data_size;
				}
			}

			void put(E &element)
			{
				put(&element,1);
			}

			void remove(int num)
			{
				if(data_size-num<1)
				{
					clear();
					return;
				}
				last_chunk_for_pos=getChunkByRelativeIndex(last_chunk_for_pos,data_size%chunk_size,-num);
				data_size-=num;
			}

			void clear()
			{
				data_size=0;
				last_chunk_for_pos=first_chunk;
			}

			E& getElement(int index)
			{
				return *(getChunkByIndex(index)->data+index%chunk_size);
			}

			E* toArray(int length)
			{
				E* arr=(E*)malloc(length*sizeof(E));
				int pos=0;
				int chunk_length=length%chunk_size==0?length/chunk_size:length/chunk_size+1;
				Chunk* this_chunk=first_chunk;
				for(int chunk_pos=0;chunk_pos<chunk_length;++chunk_pos)
				{
					if(chunk_pos==chunk_length-1)
					{
						memcpy(arr+pos,this_chunk->data,length%chunk_size);
						return arr;
					}
					memcpy(arr+pos,this_chunk->data,chunk_size);
					pos+=chunk_size;
					this_chunk=this_chunk->next;
				}
				return nullptr;
			}

			E* toArray()
			{
				return toArray(data_size);
			}

			E* toArrayCpy(int length)
			{
				E* arr=(E*)malloc(length*sizeof(E));
				int pos=0;
				int chunk_length=length%chunk_size==0?length/chunk_size:length/chunk_size+1;
				Chunk* this_chunk=first_chunk;
				for(int chunk_pos=0;chunk_pos<chunk_length;++chunk_pos)
				{
					if(chunk_pos==chunk_length-1)
					{
						int lim=length<=chunk_size?length:length%chunk_size;
						for(int i=0;i<lim;++i,++pos)
							*(arr+pos)=*(this_chunk->data+i);
						return arr;
					}
					for(int i=0;i<chunk_size;++i,++pos)
						*(arr+pos)=*(this_chunk->data+i);
					this_chunk=this_chunk->next;
				}
				return nullptr;
			}

			E* toArrayCpy()
			{
				return toArrayCpy(data_size);
			}

			void operator=(const LinkedChunkBuffer &buffer)
			{
				chunk_size=buffer.chunk_size;
				chunk_count=buffer.chunk_count;
				data_size=buffer.data_size;
				first_chunk=buffer.first_chunk;
				last_chunk=buffer.last_chunk;
				last_chunk_for_pos=buffer.last_chunk_for_pos;
			}

			void operator=(LinkedChunkBuffer &&buffer)
			{
				chunk_size=buffer.chunk_size;
				chunk_count=buffer.chunk_count;
				data_size=buffer.data_size;
				first_chunk=buffer.first_chunk;
				last_chunk=buffer.last_chunk;
				last_chunk_for_pos=buffer.last_chunk_for_pos;
				buffer.first_chunk=nullptr;
			}
		};
	}
}

#endif // ANDROMEDA_UTIL_LINKEDCHUNKBUFFER
