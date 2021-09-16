/*

  File:              pagerank.c
  Purpose:           Calculating rank of a webpage using MATLAB software

 */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "engine.h"

#define  BUFSIZE 256
#define  WEB "web.txt"

int main( void ) {
   /* ------Variables declared here ---------------------------- */

    Engine* enginePointer         = NULL; // A pointer to a MATLAB engine object
    myArray* connectivityMatrix   = NULL;
    mxArray *Web_array,*PageRank  = NULL;
    int Web_file; //set up this array with web file stuff
    int dimension; // variable for dimension

		 /* ---Starts a MATLAB process----------------------------------- */

    if ( !(enginePointer = engOpen(NULL)) ) {
      fprintf(stderr, "\nCan't start MATLAB engine\n");
      system("pause");
      return 1;
    }
    /* --- End of Starting a MATLAB process -----------------------------*/


    /* --- First, need to store data in a 2D array ----------------------*/
        
      /* Adapted from Take Home 4, to read web file into array  */
        int          dimension = 0;
        int          error = 0;

        FILE * Web_file = NULL;

        web_cell ** web      = NULL;
        
        error = fopen_s( &Web_file, WEB, "r" );

        if ( error ) {
          fprintf( stderr, "Unable to open file: %s\n", WEB );
          system( "pause" );
          return 1;
        }

        dimension = get_web_dimension(Web_file);
        printf("File loaded, with dimension size: %d\t\n", dimension);

        //Copy info from web file into array titled Web_file (Needs to be finished off)

        int totalNumberOfLinks = getNumberOfLinks();
        printf("Number of links: %d" totalNumberOfLinks);

        connectivityMatrix = fillInMatrix();
      

    /* --- End of storing data in a 2D array ----------------------------*/



		Web_array = mxCreateDoubleMatrix(dimension, dimension, mxREAL);

    memcpy((void*) mxGetPr(Web_array), (void *)Web_file, (dimension*dimension) * sizeof(int));

		if ( engPutVariable(enginePointer, "Web_array", Web_array) ) {
          fprintf(stderr, "\nCannot write Web Array to MATLAB \n");
          system("pause");
          exit(1); // Same as return 1;
        }

		if ( engEvalString(enginePointer, "dimension = size(Web_array, 1)")  ) {
          fprintf(stderr, "\nError finding Web size\n");
          system("pause");
          exit(1);
        }

		if ( engEvalString(enginePointer, "columnsums = sum(Web_array, 1)")  ) {
          fprintf(stderr, "\nError finding Column Sum\n");
          system("pause");
          exit(1);
        }

		if ( engEvalString(enginePointer, "p = 0.85")  ) {
          fprintf(stderr, "\nError setting p\n");
          system("pause");
          exit(1);
        }

		if ( engEvalString(enginePointer, "zerocolumns = find(columnsums~=0)")  ) {
          fprintf(stderr, "\nError with finding zero columns\n");
          system("pause");
          exit(1);
        }

		if ( engEvalString(enginePointer, "D = sparse( zerocolumns, zerocolumns, 1./columnsums(zerocolumns), dimension, dimension)")  ) {
          fprintf(stderr, "\nError finding D\n");
          system("pause");
          exit(1);
        }

		if ( engEvalString(enginePointer, "StochasticMatrix = ConnectivityMatrix * D")  ) {
          fprintf(stderr, "\nError making Stochastic Matrix\n");
          system("pause");
          exit(1);
        }

		if ( engEvalString(enginePointer, "[row, column] = find(columnsums==0)")  ) {
          fprintf(stderr, "\nError finding zero columns\n");
          system("pause");
          exit(1);
        }

		if ( engEvalString(enginePointer, "StochasticMatrix(:, column) = 1./dimension")  ) {
          fprintf(stderr, "\nError generating Stochastic Matrix\n");
          system("pause");
          exit(1);
        }

		if ( engEvalString(enginePointer, "Q = ones(dimension, dimension)")  ) {
          fprintf(stderr, "\nError finding Q\n");
          system("pause");
          exit(1);
        }

		if ( engEvalString(enginePointer, "TransitionMatrix = p * StochasticMatrix + (1 - p) * (Q/dimension)")  ) {
          fprintf(stderr, "\nError making Transistion Matrix \n");
          system("pause");
          exit(1);
        }

		if ( engEvalString(enginePointer, "PageRank = ones(dimension, 1)")  ) {
          fprintf(stderr, "\nError finding PageRank \n");
          system("pause");
          exit(1);
        }

		if ( engEvalString(enginePointer, "for i = 1:100 PageRank = TransitionMatrix * PageRank; end")  ) {
          fprintf(stderr, "\nError stopping Column Vector from changing \n");
          system("pause");
          exit(1);
        }

		if ( engEvalString(enginePointer, "PageRank = PageRank / sum(PageRank)")  ) {
          fprintf(stderr, "\nError getting PageRank\n");
          system("pause");
          exit(1);
        }

		printf("\nRetrieving PageRank...\n");
        if ((PageRank = engGetVariable(enginePointer,"PageRank")) == NULL) {
          fprintf(stderr, "\nFailed to retrieve PageRank\n");
          system("pause");
          exit(1);
        }
        else {
          size_t sizeOfPageRank = mxGetNumberOfElements(PageRank);
          size_t i = 0;
          size_t j = 1;
          printf("NODE  RANK\n");
          printf("---  ----\n");
          for (i = 0; i < sizeOfPageRank; ++i) {
            printf("%d  %f\n",j, *(mxGetPr(sizeofPageRank) + i) );
            j++;
          }
        }

	    if ( engOutputBuffer(enginePointer, buffer, BUFSIZE) ) {
          fprintf(stderr, "\nCan't create buffer for MATLAB output\n");
          system("pause");
          return 1;
        }
        buffer[BUFSIZE] = '\0';

        engEvalString(enginePointer, "whos"); // whos is a handy MATLAB command that generates a list of all current variables
        printf("%s\n", buffer);

        if ( engClose(enginePointer) ) {
          fprintf(stderr, "\nFailed to close MATLAB engine\n");
        }

		system("pause"); // So the terminal window remains open long enough for you to read it
        return 0; // Because main returns 0 for successful completion
}



/* --Added functions: -----------------------*/

/* --- Fill in Matrix ---------------------------------*/
double** fillInMatrix(FILE* Web_file, int dimension){
  char lineBuffer[BUFSIZE];
  double** oringinal_Web_Matrix;

  oringinal_Web_Matrix = malloc((dimension*dimension) *sizeof(double));

  while(fgets(lineBuffer, BUFSIZE, Web_file)){// as long as there's still content..

    for(int i = 0; i< dimension; i++){
      for(int j = 0; j< dimension; j++){
        Oringinal_Web_Matrix[i][j] = lineBuffer[i][j];
      }
    }
  } 
  // while loops ends when end of dimension is reached:
  return oringinal_Web_Matrix; 
}

/* --- retrives the dimensions of the web file ----------------------*/
int get_web_dimension( FILE* Web_file )  {

	int dimension = 0;
  char line_buffer[BUFFER];

	dimension = strlen( fgets ( line_buffer, BUFFER, Web_file ) );

	/* You don't need to know this.  It 'resets' the file's internal pointer to the
	   beginning of the file. */
	fseek( Web_file, 0, SEEK_SET );

    /* Checks if text file was created in Windows and contains '\r'
	   IF TRUE reduce strlen by 2 in order to omit '\r' and '\n' from each line
	   ELSE    reduce strlen by 1 in order to omit '\n' from each line */
    if ( strchr( line_buffer, '\r' ) != NULL ) {
        // INSERT CODE HERE (1 line)
        return dimension - 2;
        // return ...
    } else {
        // INSERT CODE HERE (1 line)
        return dimension - 1;
        // return ...
    }
}
