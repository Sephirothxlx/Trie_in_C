#include "trie.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

struct trie_data_t{
	unsigned int number;
	trie_pos_t root;
};

struct trie_node_t{
	trie_pos_t child;
	trie_pos_t sibling;
	char* key;
	char* ckey;
	void* value;
};

//helper function
bool walker (trie_t t, trie_pos_t pos, const char * key,void * priv){
	if(pos->value!=NULL)
		return false;	
	return true;
}

bool trie_walk (trie_t trie, trie_walk_t walkfunc, void * priv){
	if(trie==TRIE_INVALID)
		return false;
	//if the trie is empty
	if(trie->number==0)
		return true;
	if(trie->root==NULL)
		return true;
	//call walkfunc for the current node
	if(walkfunc!=NULL&&trie->root->value!=NULL&&walkfunc(trie,trie->root,trie->root->ckey,priv)==false){
		return false;
	}
	trie_pos_t temp=trie->root->child;
	//call trie_walk recursively
	while(temp!=NULL){
		//construct a temp pointer
		trie_t temp2=(trie_t)malloc(sizeof(struct trie_data_t));
		temp2->number=1;
		temp2->root=temp;
		if(trie_walk(temp2,walkfunc,priv)==false){
			free(temp2);
			return false;
		}
		//free temp pointer
		free(temp2);
		temp=temp->sibling;
	}
	return true;
}

void trie_destroy (trie_t trie, trie_free_t freefunc){
	//do nothing
	if(trie==TRIE_INVALID)
		return;
	if(trie->root==NULL){
		//free trie
		free(trie);
		return;
	}
	trie_pos_t temp=trie->root;
	//call recursively
	while(temp!=NULL){
		if(temp->child!=NULL){
			trie_t temp2=(trie_t)malloc(sizeof(struct trie_data_t));
			temp2->number=1;
			temp2->root=temp->child;
			trie_destroy(temp2,freefunc);
		}
		//free
		if(temp->key!=NULL)
			free(temp->key);
		if(temp->ckey!=NULL)
			free(temp->ckey);
		if(temp->value!=NULL&&NULL!=freefunc)
			freefunc(temp->value);
		//free the node pointer
		trie_pos_t temp3=temp->sibling;
		free(temp);
		temp=temp3;
	}
	//free the trie
	free(trie);
}

void * trie_get_value (const trie_t trie, trie_pos_t pos){
	if(trie==NULL)
		return TRIE_INVALID;
	return pos->value;
}

void trie_set_value (trie_t trie, trie_pos_t pos, void * value){
	if(trie==TRIE_INVALID)
		return;
	if(trie==NULL)
		return;
	pos->value=value;
}

trie_t trie_new (){
	trie_t trie=(trie_t)malloc(sizeof(struct trie_data_t));
	trie->number=0;
	trie->root=(trie_pos_t)malloc(sizeof(struct trie_node_t));
	//set values
	trie->root->sibling=NULL;
	trie->root->child=NULL;
	trie->root->key=NULL;
	trie->root->ckey=NULL;
	trie->root->value=NULL;
	return trie;
}

unsigned int trie_size (const trie_t trie){
	return trie->number;
}

bool trie_insert (trie_t trie, const char * str, void * newval,trie_pos_t * newpos){
	//corner case
	if(trie==TRIE_INVALID)
		return false;
	if(trie==NULL)
		return false;
	if(trie->root==NULL)
		return false;
	if(str==NULL)
		return false;
	trie_pos_t parent=trie->root;
	trie_pos_t temp=NULL;
	int x=0;
	while(true){
		temp=parent->child;
		trie_pos_t temp2=temp;
		while(temp2!=NULL){
			if(temp2->key==NULL){
				temp2=temp2->sibling;
				continue;
			}
			if(*(temp2->key)==*(str+x)){
				if(strlen(str)==x+1){
					//if the node is not NULL, we need to return false
					if(temp2->value!=NULL){
						return false;
					}else{
					//if the node is NULL, we need to return this node
						temp2->value=newval;
						temp2->ckey=(char*)malloc(sizeof(char)*(x+2));
						strcpy(temp2->ckey,str);
						//result
						if(newpos!=NULL)
							*newpos=temp2;
						trie->number++;
						return true;
					}
				}else{
					parent=temp2;
					x++;
					break;
				}
			}else{
				temp2=temp2->sibling;
			}
		}
		if(temp2==NULL){
			trie_pos_t newnode=(trie_pos_t)malloc(sizeof(struct trie_node_t));
			newnode->key=(char*)malloc(sizeof(char));
			*(newnode->key)=*(str+x);
			newnode->child=NULL;
			//update the linkedlist
			if(temp!=NULL){
			//insert the new node after the first node
				newnode->sibling=temp->sibling;
				temp->sibling=newnode;
			}else{
				newnode->sibling=NULL;
				temp=newnode;
			}
			//use the parent pointer
			parent->child=temp;
			if(strlen(str)==x+1){
				newnode->value=newval;
				newnode->ckey=(char*)malloc(sizeof(char)*(x+2));
				strcpy(newnode->ckey,str);
				//result
				if(newpos!=NULL)
					*newpos=newnode;
				break;
			}else{
				newnode->value=NULL;
				newnode->ckey=NULL;
				x++;
			}
			//update the parent pointer
			parent=newnode;
		}
	}
	trie->number++;
	return true;
}
      
trie_pos_t trie_find (const trie_t trie, const char * key){
	//corner case
	if(trie==TRIE_INVALID)
		return TRIE_INVALID_POS;
	if(trie->root==NULL)
		return TRIE_INVALID_POS;
	if(key==NULL)
		return TRIE_INVALID_POS;
	if(strlen(key)==0)
		return TRIE_INVALID_POS;
	trie_pos_t root=trie->root;
	if(root->child==NULL)
		return TRIE_INVALID_POS;
	trie_pos_t temp=root->child;
	int x=0;
	while(temp!=NULL){
		trie_pos_t temp2=temp;
		while(temp2!=NULL){
			if(temp2->key==NULL){
				temp2=temp2->sibling;
				continue;
			}
			if(*(temp2->key)!=*(key+x)){
				temp2=temp2->sibling;
			}else{
				if(strlen(key)==x+1){
					//if the node is not deleted
					if(temp2->value!=NULL)
						return temp2;
					else
					//if the node is deleted
						return TRIE_INVALID_POS;
				}
				temp=temp2->child;
				x++;
				break;
			}
		}
		if(temp2==NULL)
			break;
	}
	return TRIE_INVALID_POS;
}

bool trie_remove (trie_t trie, const char * key, void ** data){
	//find the node
	trie_pos_t node=trie_find(trie,key);
	if(node==TRIE_INVALID_POS)
		return false;
	if(node->value==NULL)
		return false;
	if(data!=NULL)
		*data=node->value;
	node->value=NULL;
	free(node->ckey);
	node->ckey=NULL;
	trie->number--;
	//check if the subtree needs to be deleted
	trie_t t=(trie_t)malloc(sizeof(struct trie_data_t));
	t->number=1;
	trie_pos_t temp=(trie_pos_t)malloc(sizeof(struct trie_node_t));
	t->root=temp;
	temp->sibling=NULL;
	temp->child=node->child;
	temp->key=NULL;
	temp->ckey=NULL;
	temp->value=NULL;
	//if it needs to be deleted
	if(trie_walk(t,walker,NULL)){
		trie_destroy(t,NULL);
		node->child=NULL;
	}else{
	//if not
		free(temp);
		free(t);
	}
	return true;
}

