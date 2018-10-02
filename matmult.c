#include <stdio.h>
#include <stdlib.h>    /* exit, malloc, calloc, etc. */
#include <string.h>
#include <getopt.h>    /* getopt */

#include <likwid.h>

#include "matriz.h"

/**
 * Exibe mensagem de erro indicando forma de uso do programa e termina
 * o programa.
 */

static void usage(char *progname)
{
  fprintf(stderr, "Forma de uso: %s [ -n <ordem> ] [ -p <passo> ] [ -m <inicio> -M <fim> ] \n", progname);
  exit(1);
}



/**
 * Programa principal
 * Forma de uso: matmult [ -n <ordem> ] [ -p <passo> ] [ -m <inicio> -M <fim> ]
 * -n <ordem>: ordem da matriz quadrada e dos vetores
 * -m <inicio> e -M <fim> : permite repetir as operações com as matrizes e vetores 
 *                          com diversas ordens, com 'inicio <= n <= fim'.
 *                          O passo entre valores de 'n' neste caso é dada pela
 *                          opção '-p <passo>' 
 *
 */

int main (int argc, char *argv[]) 
{

  LIKWID_MARKER_INIT;
  int c, passo=DEF_PASSO, n=DEF_LINS, m=DEF_MIN, M=DEF_MAX, hasM=0, hasm=0;
  double norma;
  
  MatPtr mPtr;
  MatRow mRow;
  MatCol mCol;
  Vetor vet, resPtr, resRow, resCol;
  
  /* =============== TRATAMENTO DE LINHA DE COMAANDO =============== */

  char *opts = "n:m:M:p:";
  c = getopt (argc, argv, opts);
  
  while ( c != -1 ) {
    switch (c) {
    case 'n':  n = atoi(optarg);              break;
    case 'm':  m = atoi(optarg);      hasm=1; break;
    case 'M':  M = atoi(optarg);      hasM=1; break;
    case 'p':  passo = atoi(optarg);          break;
    default:   usage(argv[0]);
    }
      
    c = getopt (argc, argv, opts);
  }

  if ( hasm != hasM)   usage(argv[0]);
  
  /* ================ FIM DO TRATAMENTO DE LINHA DE COMANDO ========= */
  
  
  srand(20182);
  
  if (!hasm && !hasM) m = M = n;

  for (n=m; n <= M; n+=passo) {
    mPtr = geraMatPtr (n, n);
    mRow = geraMatRow (n, n);
    mCol = geraMatCol (n, n);
    vet = geraVetor (n);
    
#ifdef DEBUG
    prnMatPtr (mPtr, n, n);
    prnMatRow (mRow, n, n);
    prnMatCol (mCol, n, n);
    prnVetor (vet, n);
    printf ("=================================\n\n");
#endif /* DEBUG */

    resPtr = (double *) calloc (n, sizeof(double));
    LIKWID_MARKER_START("matptr");
    multMatPtrVet (mPtr, vet, n, n, resPtr);
    LIKWID_MARKER_STOP("matptr");

    resRow = (double *) calloc (n, sizeof(double));
    LIKWID_MARKER_START("matrow");
    multMatRowVet (mRow, vet, n, n, resRow);
    LIKWID_MARKER_STOP("matrow");
    
    resCol = (double *) calloc (n, sizeof(double));
    LIKWID_MARKER_START("matcol");
    multMatColVet (mCol, vet, n, n, resCol);
    LIKWID_MARKER_STOP("matcol");
    
    norma = normaMax(resRow, resPtr, n);

    norma = normaEucl(resCol, n);
  

#ifdef DEBUG
    prnVetor (resPtr, m);
    prnVetor (resRow, m);
    prnVetor (resCol, m);
#endif /* DEBUG */
  }
  LIKWID_MARKER_CLOSE;
}

