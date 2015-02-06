RC initBufferPool(BM_BufferPool *const bm,const char *const pageFileName,const int numPages,ReplacementStrategy strategy,void *stratData)
{
	int fd,i=0;
	if((fd=open(pageFileName,O_RDWR))==-1)
		return RC_FILE_NOT_FOUND;
	
	if(curPools>=MAX_POOLS)
		return FALSE;
	
	record[curPools].numRead=0;
	record[curPools].numWrite=0;
	record[curPools].freeAccess=TRUE;
	record[curPools].rPos=0;
	
	BufferFrame *bf.*tempBf;
	(*bm).pageFileName=pageFileName;
	(*bm).numPages=numPages;
	(*bm).strategy=strategy;
	
	bf=(BufferFrame*)malloc(sizeof(BufferFrame));
	bf->fixcount=0;
	bf->dirty=FALSE;
	bf->recordPos=curPools;
	bf->ph.pageNum=NO_PAGE;
	bf->ph.data=(char *)malloc(PAGE_SIZE * sizeof(char));
	bf->next=NULL;
	
	(*bm).mgmtData=bf;
	
	for(i=0;i<numPages-1;i++)
	{
		tempBf=(BufferFrame*)malloc(sizeof(BufferFrame));
		tempBf->fixcount=0;
		tempBf->dirty=FALSE;
		tempBf->recordPos=curPools;
		tempBf->ph.pageNum=NO_PAGE;
		tempBf->ph.data=(char *)malloc(PAGE_SIZE * sizeof(char));
		tempBf->next=NULL;
		
		bf->next=tempBf;
		bf=bf->next;
	}
	close(fd);
	curPools++;
	return RC_OK;
}

RC pinPage(BM_BufferPool *const bm,BM_PageHandle *const page,const PageNumber pageNum)
{
	int i;
	BufferFrame *bf,*tempBf,*p,*q;
	
	tempBf=bm->mgmtData;
	for(i=0;i<bm->numPages;i++){
		if(tempBf->ph.pageNum==pageNum)
			break;
		tempBf=tempBf->next;
	}
	bf=tempBf;
	
	if(bf==NULL)
	{
		bf=addPagetoPool(bm,pageNum);
		if(bf==NULL)
			return FALSE;
	}
	
	switch(bm->strategy)
	{
		case RS_FIFO:
			break;
	
	}
	
	bf->fixcount++;
	page->data=(char*)malloc(sizeof(char)*PAGE_SIZE);
	
	while(!record[bf->recordPos].freeAccess)
		sleep(0.5);
	record[bf->recordPos].freeAccess=FALSE;
	strcpy(page->data,bf->ph.data);
	record[bf->recordPos].freeAccess=TRUE;
	page->pageNum=bf->ph.pageNum;
	
	return RC_OK;	
}