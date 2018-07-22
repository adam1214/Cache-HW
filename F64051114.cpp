#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<map>
#include<algorithm>
#include<deque>
#include<vector>
FILE *output,*input;
using namespace std;
int main(int argc,char *argv[])
{
	vector <int> hit;
	vector <int> miss;
	deque<unsigned int> ideq;
	deque<unsigned int>::iterator pos;
	map<unsigned int,unsigned int> index_Tag;
	map<unsigned int,unsigned int>::iterator iter;
	int line_cnt=0,cache_size,block_size,associativity,alogorithm,block_cnt,set_cnt,hit_cnt=0,miss_cnt=0;
	int direct_Tag,direct_index,direct_block_offset;
	int cnt[10]={0};
	char ch;
	unsigned int add,Tag,index;
	input=fopen(argv[2],"r");
	output=fopen(argv[4],"w");

	while((ch=fgetc(input))!=EOF)
	{
		if(ch=='\n')
			++line_cnt;
	}

	fclose(input);
	input=fopen(argv[2],"r");
	for(int i=0;i<4;i++)
	{
		if(i==0)
		{
			fscanf(input,"%d",&cache_size);
		}
		else if(i==1)
		{
			fscanf(input,"%d",&block_size);
			block_cnt=cache_size*1024/block_size;
			ideq=deque<unsigned int>(block_cnt);
			ideq.clear();
		}
		else if(i==2)
		{
			fscanf(input,"%d",&associativity);
			if(associativity==0) //direct-mapped
			{
				set_cnt=block_cnt;
			}
			else if(associativity==1) //four-way set associative
			{
				set_cnt=block_cnt/4;
			}
			else if(associativity==2) //fully associative
			{
				set_cnt=1;
			}
		}
		else if(i==3)
		{
			fscanf(input,"%d",&alogorithm); //FIFO=0,LRU=1
		}
	}
	//define four way deque
	deque<unsigned int> ideq_four[set_cnt];
	for(int i=0;i<set_cnt;i++)
	{
		ideq_four[i]=deque<unsigned int>(4);
		ideq_four[i].clear();
	}

	for(int i=4;i<line_cnt;i++)
	{
		direct_index=log(block_cnt)/log(2);
		direct_block_offset=log(block_size)/log(2);
		direct_Tag=32-direct_index-direct_block_offset;
		fscanf(input," %x",&add);
		//printf("%x\n",add);
		if(associativity==0) //direct
		{
			Tag=add>>direct_index+direct_block_offset;
			index=add<<direct_Tag>>(direct_Tag+direct_block_offset);
			//find the same index in the map 
			iter=index_Tag.find(index);
			if(iter!=index_Tag.end()) //if find this index
			{
				if(iter->second==Tag) //check Tag
				{
					hit_cnt++;
					hit.push_back(i-3);
				}
				else
				{
					//clear this element and insert new one
					index_Tag.erase(iter);
					index_Tag.insert(pair<unsigned int,unsigned int>(index,Tag));
					miss_cnt++;
					miss.push_back(i-3);
				}
			}
			//insert map element
			else
			{
				index_Tag.insert(pair<unsigned int,unsigned int>(index,Tag));
				miss_cnt++;
				miss.push_back(i-3);
			}
		}
		else if(associativity==1) //4-way
		{
			Tag=add>>(direct_index+direct_block_offset-2);
			index=add<<(direct_Tag+2)>>(direct_Tag+direct_block_offset+2);
			//index determines to find which set(row of deque)
			pos=find(ideq_four[index].begin(),ideq_four[index].end(),Tag);
			if(pos!=ideq_four[index].end()) //find success
			{
				if(alogorithm==1) //LRU
				{
					int temp=*pos;
					ideq_four[index].erase(pos);
					ideq_four[index].push_back(temp);
					hit_cnt++;
					hit.push_back(i-3);
				}
				else //FIFO
				{
					hit_cnt++;
					hit.push_back(i-3);
				}

			}
			else //find failed
			{
				if(ideq_four[index].size()==4) //container is full
				{
					if(alogorithm==0) //FIFO
					{
						ideq_four[index].pop_back();
						ideq_four[index].push_front(Tag);
						miss_cnt++;
						miss.push_back(i-3);
					}
					else //LRU
					{
						ideq_four[index].pop_front();
						ideq_four[index].push_back(Tag);
						miss_cnt++;
						miss.push_back(i-3);
					}
				}
				else
				{
					if(alogorithm==1) //LRU
					{
						ideq_four[index].push_back(Tag);
						miss_cnt++;
						miss.push_back(i-3);
					}
					else //FIFO
					{
						ideq_four[index].push_front(Tag);
						miss_cnt++;
						miss.push_back(i-3);
					}
				}                          
			}
		}
		else if(associativity==2) //fully
		{
			Tag=add>>direct_block_offset;
			pos=find(ideq.begin(),ideq.end(),Tag);
			if(pos!=ideq.end()) //find success
			{
				if(alogorithm==1) //LRU
				{
					int temp=*pos;
					ideq.erase(pos);
					ideq.push_back(temp);
					hit_cnt++;
					hit.push_back(i-3);
				}
				else //FIFO
				{
					hit_cnt++;
					hit.push_back(i-3);
				}
			}
			else //find failed
			{
				if(ideq.size()==block_cnt) //container is full
				{
					if(alogorithm==0) //FIFO
					{
						ideq.pop_back();
						ideq.push_front(Tag);
						miss_cnt++;
						miss.push_back(i-3);
					}
					else //LRU
					{
						ideq.pop_front();
						ideq.push_back(Tag);
						miss_cnt++;
						miss.push_back(i-3);
					}
				}
				else
				{
					if(alogorithm==1) //LRU
					{
						ideq.push_back(Tag);
						miss_cnt++;
						miss.push_back(i-3);
					}
					else //FIFO
					{
						ideq.push_front(Tag);
						miss_cnt++;
						miss.push_back(i-3);
					}
				}
			}
		}
	}
	fprintf(output,"Hits instructions: ");
	for(int i=0;i<hit.size();i++)
	{
		if(i==0)
			fprintf(output,"%d",hit[i]);
		else
			fprintf(output,",%d",hit[i]);
	}
	fprintf(output,"\nMisses instructions: ");
	for(int i=0;i<miss.size();i++)
	{
		if(i==0)
			fprintf(output,"%d",miss[i]);
		else
			fprintf(output,",%d",miss[i]);
	}
	fprintf(output,"\nMiss rate: %g\n",(double)miss_cnt/(hit_cnt+miss_cnt));

	fclose(input);
	fclose(output);
	return 0;	
}
