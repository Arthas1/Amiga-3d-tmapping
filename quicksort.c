#include <string.h>

int quicksort(int number[], int faceindex[], int first,int last)
{

int pivot = first;
    int i = first;
    int j = last;
    int temp = number[i];
    int swap = faceindex[i];


 // DESCENDING
//   if( first < last)
//          {
//                 while(i < j)
//                 {
//                        while(number[j] < temp && i < j)
//                        {
//                               j--;
//                        }
//                        number[i] = number[j];
//
//						faceindex[i] = faceindex[j];
//
//                  //memcpy(&faces[i], &faces[j], sizeof(faces[j]));
//
//
//                 while(number[i] >= temp && i < j)
//                        {
//                               i++;
//                        }
//
//                 number[j] = number[i];
//
//				 faceindex[j] = faceindex[i];
//
//                 //memcpy(&faces[j], &faces[i], sizeof(faces[i]));
//
//                 }
//
//                 number[i] = temp;
//
//                 faceindex[i] = swap;
//
//                 //memcpy(&faces[i], &swap, sizeof(swap));
//
//
//
//          quicksort(number, faceindex, first, i - 1);
//          quicksort(number, faceindex, j + 1, last);
//          }
//   }
//
//







////	 Ascending
//
//
//
//


							 if(first<last){
							      pivot=first;
							      i=first;
							      j=last;


							      while(i<j){
							         while(number[i]<=number[pivot]&&i<last)
							            i++;
							         while(number[j]>number[pivot])
							            j--;
							         if(i<j){
							            temp=number[i];
							            number[i]=number[j];
							            number[j]=temp;


							             memcpy(&swap, &faceindex[i], sizeof(faceindex[i]));
							             memcpy(&faceindex[i], &faceindex[j], sizeof(faceindex[j]));
							             memcpy(&faceindex[j], &swap, sizeof(swap));



							         }
							      }

							      temp=number[pivot];
							      number[pivot]=number[j];
							      number[j]=temp;


							      	  	  	  	  	  	  	   memcpy(&swap, &faceindex[pivot], sizeof(faceindex[pivot]));

							     						       memcpy(&faceindex[pivot], &faceindex[j], sizeof(faceindex[j]));

							     						       memcpy(&faceindex[j], &swap, sizeof(swap));




							      quicksort(number, faceindex,first,j-1);
							      quicksort(number, faceindex, j+1,last);


			}
return 0;
}


// BOUBLE SORTING

// float swap[3];
//
//						 for(int k=0; k<nrFaces; k++)
//					        {
//
//							    for (int d = 0 ; d < nrFaces - 1; d++)
//							    {
//
//						    	 for (int e = 0 ; e < nrFaces - d-1; e++)
//							    	 {
//
//						    		 int referSort1 = faces[e][0];
//						    		 int referSort2 = faces[e][1];
//						    		 int referSort3 = faces[e][2];
//
//						    		 int referSort1B = faces[e+1][0];
//						    		 int referSort2B = faces[e+1][1];
//						    		 int referSort3B = faces[e+1][2];
//
//
//
//						    		 float zBuff1 = result[referSort1][2] + result[referSort2][2] + result[referSort3][2] / 3.0f;    		// Z z kazdych 3ech punktow poligonu 1
//						    		 float zBuff2 = result[referSort1B][2] + result[referSort2B][2] + result[referSort3B][2] / 3.0f;	// Z z kazdych trzech punktow poligonu 2
//
//							    		 if (zBuff1 < zBuff2){
//
//							    			 memcpy(&swap, &faces[e], sizeof(faces[e]));
//
//							    			 memcpy(&faces[e], &faces[e+1], sizeof(faces[e+1]));
//
//							    			 memcpy(&faces[e+1], &swap, sizeof(swap));
//
//							      }
//							    }
//							  }
//					        }

