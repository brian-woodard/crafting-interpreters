
#pragma once

#include <stdio.h>
#include <sys/stat.h>
#include <memory.h>
#include <stdint.h>

#define ArrayCount(array) sizeof(array)/sizeof(array[0])

///////////////////////////////////////////////////////////////////////////////
// File Functions
///////////////////////////////////////////////////////////////////////////////
struct TBuffer
{
   uint8_t* Data;
   uint32_t Count;
};

TBuffer ReadEntireFile(const char* FileName)
{
   TBuffer Result = {};

   FILE *File = fopen(FileName, "rb");
   if (File)
   {
      struct stat Stat;
      stat(FileName, &Stat);

      Result.Data = new uint8_t[Stat.st_size];
      Result.Count = Stat.st_size;

      if (Result.Data)
      {
         if (fread(Result.Data, Result.Count, 1, File) != 1)
         {
            fprintf(stderr, "ERROR: Unable to read \"%s\".\n", FileName);
            Result.Data = nullptr;
            Result.Count = 0;
            delete [] Result.Data;
         }
      }

      fclose(File);
   }
   else
   {
      fprintf(stderr, "ERROR: Unable to open \"%s\".\n", FileName);
   }

   return Result;
}

///////////////////////////////////////////////////////////////////////////////
// Hash Table
///////////////////////////////////////////////////////////////////////////////
template <typename K, typename V>                                               
class HashTable                                                                 
{                                                                               
public:                                                                         
                                                                                
   static constexpr int INITIAL_BUCKET_SIZE = 1024;                             
                                                                                
   typedef uint32_t(*HashFnPtr)(K Key);                                         
   typedef void(*PrintFnPtr)(uint32_t Index, K Key, V Value);                   
                                                                                
   HashTable() : mBucketSize(INITIAL_BUCKET_SIZE), mTable(nullptr), mHashFn(nullptr), mPrintFn(nullptr)
   {                                                                            
      mTable = new TKeyValuePair[mBucketSize];                                  
      memset(mTable, 0, sizeof(TKeyValuePair)*mBucketSize);                     
   }                                                                            
                                                                                
   ~HashTable()                                                                 
   {                                                                            
      if (mTable)                                                               
      {                                                                         
         // delete external chains                                              
         for (uint32_t i = 0; i < mBucketSize; i++)                             
         {                                                                      
            if (mTable[i].Used)                                                 
            {                                                                   
               TKeyValuePair* curr = mTable[i].Next;                            
               while (curr)                                                     
               {                                                                
                  TKeyValuePair* tmp = curr;                                    
                  curr = curr->Next;                                            
                  delete tmp;                                                   
               }                                                                
            }                                                                   
         }                                                                      
                                                                                
         delete [] mTable;                                                      
      }                                                                         
   }                                                                            
                                                                                
   float LoadFactor()                                                           
   {                                                                            
      return (float)Size()/(float)mBucketSize;                                  
   }

   void Print()                                                                 
   {                                                                            
      if (mTable)                                                               
      {                                                                         
         for (uint32_t i = 0; i < mBucketSize; i++)                             
         {                                                                      
            if (mTable[i].Used)                                                 
            {                                                                   
               if (mPrintFn)                                                    
                  mPrintFn(i, mTable[i].Key, mTable[i].Value);                  
               else                                                             
                  printf("Bucket %d: Key %d Value %d\n", i, mTable[i].Key, mTable[i].Value);
                                                                                
               TKeyValuePair* curr = mTable[i].Next;                            
               while (curr)                                                     
               {                                                                
                  if (mPrintFn)                                                 
                     mPrintFn(i, curr->Key, curr->Value);                       
                  else                                                          
                     printf("Bucket %d: Key %d Value %d\n", i, curr->Key, curr->Value);
                                                                                
                  curr = curr->Next;                                            
               }                                                                
            }                                                                   
         }                                                                      
      }                                                                         
   }                                                                            
                                                                                
   void SetHashTableFn(HashFnPtr HashFn)                                        
   {                                                                            
      mHashFn = HashFn;                                                         
   }                                                                            
                                                                                
   void SetPrintFn(PrintFnPtr PrintFn)                                          
   {                                                                            
      mPrintFn = PrintFn;                                                       
   }                                                                            
                                                                                
   uint32_t Size()                                                              
   {                                                                            
      uint32_t size = 0;                                                        
                                                                                
      for (uint32_t i = 0; i < mBucketSize; i++)                                
      {                                                                         
         if (mTable[i].Used)                                                    
         {                                                                      
            size++;                                                             
                                                                                
            TKeyValuePair* curr = mTable[i].Next;                               
            while (curr)                                                        
            {                                                                   
               size++;                                                          
               curr = curr->Next;                                               
            }                                                                   
         }                                                                      
      }                                                                         
                                                                                
      return size;                                                              
   }

   V At(const K Key)
   {
      uint32_t index = 0;                                                       
                                                                                
      if (mHashFn)                                                              
      {                                                                         
         index = mHashFn(Key) % mBucketSize;                                    
                                                                                
         // chaining                                                            
         if (mTable[index].Used)                                             
         {                                                                   
            if (mTable[index].Key != Key)                                          
            {
               TKeyValuePair* prev = &mTable[index];                            
               TKeyValuePair* curr = mTable[index].Next;                        
                                                                                
               while (curr)                                                     
               {                                                                
                  if (curr->Key == Key)                                         
                  {                                                             
                     return curr->Value;                                        
                  }                                                             
                  prev = curr;                                                  
                  curr = curr->Next;                                            
               }                                                                
                                                                                
               prev->Next = new TKeyValuePair;                                  
               curr = prev->Next;                                               
                                                                                
               return curr->Value;                                              
            }
         }                                                                   
      }                                                                         
                                                                                
      return mTable[index].Value;                                               
   }

   V& operator[](const K Key)                                                   
   {                                                                            
      uint32_t index = 0;                                                       
                                                                                
      if (mHashFn)                                                              
      {                                                                         
         index = mHashFn(Key) % mBucketSize;                                    
                                                                                
         // chaining                                                            
         if (mTable[index].Used)                                             
         {                                                                   
            if (mTable[index].Key != Key)                                          
            {
               TKeyValuePair* prev = &mTable[index];                            
               TKeyValuePair* curr = mTable[index].Next;                        
                                                                                
               while (curr)                                                     
               {                                                                
                  if (curr->Key == Key)                                         
                  {                                                             
                     return curr->Value;                                        
                  }                                                             
                  prev = curr;                                                  
                  curr = curr->Next;                                            
               }                                                                
                                                                                
               prev->Next = new TKeyValuePair;                                  
               curr = prev->Next;                                               
                                                                                
               curr->Key = Key;                                                 
               curr->Used = true;                                               
               curr->Next = nullptr;                                            
                                                                                
               return curr->Value;                                              
            }
         }                                                                   
         else                                                                
         {                                                                   
            mTable[index].Key = Key;                                         
            mTable[index].Used = true;                                       
         }                                                                   
      }                                                                         
                                                                                
      return mTable[index].Value;                                               
   }                                                                            
                                                                                
private:                                                                        
                                                                                
   struct TKeyValuePair                                                         
   {                                                                            
      K              Key;                                                       
      V              Value;                                                     
      TKeyValuePair* Next;                                                      
      bool           Used;                                                      
   };                                                                           
                                                                                
   uint32_t       mBucketSize;                                                  
   TKeyValuePair* mTable;                                                       
   HashFnPtr      mHashFn;                                                      
   PrintFnPtr     mPrintFn;                                                     
};
