#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "index.h"


struct record_ {
  int clave;
  int num_registros;
  long *registros;
};

struct index_ {
	type_t type;
  int num_claves;
  char *file;
  record_t **records;
};





/*
   Function: int index_create(char *path, int type)

   Creates a file for saving an empty index. The index is initialized
   to be of the specific tpe (in the basic version this is always INT)
   and to contain 0 entries.

   Parameters:
   path:  the file where the index is to be created
   type:  the type of the index (always INT in this version)

   Returns:
   1:   index created
   0:   parameter error or file creation problem. Index not created.
 */
int index_create(char *path, type_t type) {
  FILE *f;
  int num_claves = 0;

  if(!path){
    return 0;
  }

  f = fopen(path, "wb");
  if(!f){
    return 0;
  }

  fwrite(&type, sizeof(type_t), 1, f);
  fwrite(&num_claves, sizeof(int), 1, f);

  fclose(f);
	return 1;
}



/*
   Opens a previously created index: reads the contents of the index
   in an index_t structure that it allocates, and returns a pointer to
   it (or NULL if the files doesn't exist or there is an error).

   NOTE: the index is stored in memory, so you can open and close the
   file in this function. However, when you are asked to save the
   index, you will not be given the path name again, so you must store
   in the structure either the FILE * (and in this case you must keep
   the file open) or the path (and in this case you will open the file
   again).

   Parameters:
   path:  the file where the index is

   Returns:
   pt:   index opened
   NULL: parameter error or file opening problem. Index not opened.

 */
index_t* index_open(char* path) {
  FILE *f;
  index_t *index;
  type_t type;
  int num_claves, i, j, aux;

  if(!path){
    return NULL;
  }

  index = (index_t*)malloc(sizeof(index_t));
  if(index==NULL){
    return NULL;
  }

  f = fopen(path, "a+b");
  if(!f){
    free(index);
    return NULL;
  }

  index->file = path;

  aux = fread(&type, sizeof(type_t), 1, f);
  index->type = type;

  aux = fread(&num_claves, sizeof(int), 1, f);
  index->num_claves = num_claves;

  index->records = (record_t**)malloc(num_claves*sizeof(record_t*));
  if(!index->records){
    free(index);
    return NULL;
  }


  for(i = 0; i < num_claves; i++){
    index->records[i] = (record_t*)malloc(sizeof(record_t));
    aux = fread(&(index->records[i]->clave), sizeof(int), 1, f);
    aux = fread(&(index->records[i]->num_registros), sizeof(int), 1, f);
    index->records[i]->registros = (long*)malloc(index->records[i]->num_registros*sizeof(long));
    for(j = 0; j < index->records[i]->num_registros; j++){
      aux = fread(&(index->records[i]->registros[j]), sizeof(long), 1, f);
    }
  }
  fclose(f);
  return index;
}


/*
   int index_save(index_t* index);

   Saves the current state of index in the file it came from. Note
   that the name of the file in which the index is to be saved is not
   given.  See the NOTE to index_open.

   Parameters:
   index:  the index the function operates upon

   Returns:
   1:  index saved
   0:  error saving the index (cound not open file)

*/




int index_save(index_t* idx) {
  int i,j;
  FILE *f;
  if(!idx){
    return 0;
  }

  f = fopen(idx->file, "wb");

  fwrite(&idx->type, sizeof(int), 1, f);
  fwrite(&idx->num_claves, sizeof(int), 1, f);

  for (i=0;i<idx->num_claves; i++){
    fwrite(&idx->records[i]->clave, sizeof(int), 1, f);
    fwrite(&idx->records[i]->num_registros, sizeof(int), 1, f);

    for(j=0; j<idx->records[i]->num_registros; j++){
      fwrite(&idx->records[i]->registros[j], sizeof(long), 1, f);
    }
  }
  fclose(f);
  return 1;
}


/*
   Function: int index_put(index_t *index, int key, long position);

   Puts a pair key-positionition in the index. Note that the key may be
   present in the index or not... you must manage both situation. Also
   remember that the index must be kept ordered at all times.

   Parameters:
   index:  the index the function operates upon
   key: the key of the record to be indexed (may or may not be already
        present in the index)
   position: the positionition of the corresponding record in the table
        file. This is the datum that we will want to recover when we
        search for the key.

   Return:
   n>0:  after insertion the file now contains n unique keys
   0:    error inserting the key

*/
int binary_search(int *array, int key, int n) {
 int low = 0,i=0;
 int high = n-1;
 int middle = 0;

 if (n==0){
   return -1;
 }
 while(low <= high) {

   middle = (low+high)/2;

   if (array[middle] == key){
     return middle; // clave encontrada
   }
   else if (key < array[middle]){
     high = middle -1;
   }
   else{
     low = middle +1;
   }
 }
  return -(middle+2); // clave no encontrada
}

int index_put(index_t *idx, int key, long pos) {
  int position, *array, i, j;
  record_t *record, *rec_aux;
  if(!idx ){
    return 0;
  }

  array = (int*)malloc(sizeof(int)*idx->num_claves);

  for(i=0; i<idx->num_claves;i++){
    array[i]= idx->records[i]->clave;
  }

  position = binary_search(array, key, idx->num_claves);

  if (position >= 0) {
    idx->records[position]->registros = (long*)realloc(idx->records[position]->registros, (idx->records[position]->num_registros+1)*sizeof(long));
    idx->records[position]->registros[idx->records[position]->num_registros] = pos;
    idx->records[position]->num_registros++;
    for(i=0;pos>idx->records[position]->registros[i];i++);
      for(j=i;(j+1)<idx->records[position]->num_registros;j++){
        idx->records[position]->registros[j+1]=idx->records[position]->registros[j];
      }
      idx->records[position]->registros[i]=pos;
  }
  else {
    if(position != -1){
      position = -(position+1);
    }
    else{
      position = 0;
    }
    record = (record_t*)malloc(sizeof(record_t));
    record->clave = key;
    record->num_registros = 1;
    record->registros = (long*)malloc(sizeof(long));
    record->registros[0] = pos;

    idx->records = (record_t**)realloc(idx->records, (idx->num_claves+1)*sizeof(record_t*));
    idx->num_claves++;

    for(i=position;(i+1)<idx->num_claves;i++){
      idx->records[i+1]=idx->records[i];
    }

    idx->records[position] = record;
  }
  free(array);
  return idx->num_claves;
}


/*
   Function: long **index_get(index_t *index, int key, int* npositions);

   Retrieves all the positionitions associated with the key in the index.

   Parameters:
   index:  the index the function operates upon
   key: the key of the record to be searched
   npositions: output paramters: the number of positionitions associated to this key

   Returns:

   position: an array of *npositions long integers with the positionitions associated
        to this key
   NULL: the key was not found

   NOTE: the parameter npositions is not an array of integers: it is
   actually an integer variable that is passed by reference. In it you
   must store the number of elements in the array that you return,
   that is, the number of positionitions associated to the key. The call
   will be something like this:

   int n
   long **positions = index_get(index, key, &n);

   for (int i=0; i<n; i++) {
       Do something with positions[i]
   }

   ANOTHER NOTE: remember that the search for the key MUST BE DONE
   using binary search.

   FURTHER NOTE: the pointer returned belongs to this module. The
   caller guarantees that the values returned will not be changed.

*/
long *index_get(index_t *idx, int key, int* npositions) {
  int pos,i;
  int *array;
  long *positions;

  if(!idx){
    return NULL;
  }

  array = (int*)malloc(sizeof(int)*idx->num_claves);

  for(i=0; i<idx->num_claves;i++){
    array[i]= idx->records[i]->clave;
  }

  pos = binary_search(array, key, idx->num_claves);

  if (pos <0){
    free (array);
    return NULL;
  }

  *npositions = idx->records[pos]->num_registros;

  positions = (long*)malloc(*npositions*sizeof(long));

  for(i = 0; i < idx->records[pos]->num_registros; i++){
    positions[i] = idx->records[pos]->registros[i];
  }

  free(array);
  return positions;
}


/*
   Closes the index by freeing the allocated resources. No operation
   on the index will be positionsible after calling this function.

   Parameters:
   index:  the index the function operates upon

   Returns:
   Nothing

   NOTE: This function does NOT save the index on the file: you will
   have to call the function index_save for this.
*/
void index_close(index_t *idx) {
  int i;
  if (!idx){
    return;
  }

  if(idx->records != NULL){
    for(i=0; i<idx->num_claves;i++){
      if(idx->records[i] != NULL){
        if(idx->records[i]->registros != NULL){
          free(idx->records[i]->registros);
        }
        free(idx->records[i]);
      }
    }

    free(idx->records);
  }

  free(idx);
}


/*
  Function: long **index_get_order(index_t *index, int n, int* npositions);

  Function useful for debugging but that should not be used otherwise:
  returns the nth record in the index. DO NOT USE EXCEPT FOR
  DEBUGGING. The test program uses it.

   Parameters:
   index:  the index the function operates upon
   n: number of the record to be returned
   npositions: output paramters: the number of positionitions associated to this key

   Returns:

   position: an array of *npositions long integers with the positionitions associated
        to this key
   NULL: the key was not found


   See index_get for explanation on npositions and position: they are the same stuff
*/
long *index_get_order(index_t *idx, int n, int* npositions) {
  int pos,i;
  long *positions;

  if(!idx){
    return NULL;
  }

  *npositions = idx->records[n]->num_registros;

  positions = (long*)malloc(*npositions*sizeof(long));

  for(i = 0; i < idx->records[n]->num_registros; i++){
    positions[i] = idx->records[n]->registros[i];
  }

  return positions;
}
