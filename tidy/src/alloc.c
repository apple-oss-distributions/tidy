/* alloc.c -- Default memory allocation routines.

  (c) 1998-2005 (W3C) MIT, ERCIM, Keio University
  See tidy.h for the copyright notice.

  CVS Info :

    $Author$ 
    $Date$ 
    $Revision$ 

*/

#include "tidy.h"
#include "forward.h"

#ifdef __APPLE__
#include <dispatch/dispatch.h>
#include <mach-o/dyld_priv.h>
#endif

static TidyMalloc  g_malloc  = NULL;
static TidyRealloc g_realloc = NULL;
static TidyFree    g_free    = NULL;
static TidyPanic   g_panic   = NULL;

Bool TIDY_CALL tidySetMallocCall( TidyMalloc fmalloc )
{
  if (linkedOnOrAfter2024EReleases())
      return no;
  g_malloc  = fmalloc;
  return yes;
}
Bool TIDY_CALL tidySetReallocCall( TidyRealloc frealloc )
{
  if (linkedOnOrAfter2024EReleases())
      return no;
  g_realloc = frealloc;
  return yes;
}
Bool TIDY_CALL tidySetFreeCall( TidyFree ffree )
{
  if (linkedOnOrAfter2024EReleases())
      return no;
  g_free    = ffree;
  return yes;
}
Bool TIDY_CALL tidySetPanicCall( TidyPanic fpanic )
{
  g_panic   = fpanic;
  return yes;
}

void FatalError( ctmbstr msg )
{
  if ( g_panic )
    g_panic( msg );
  else
  {
    /* 2 signifies a serious error */
    fprintf( stderr, "Fatal error: %s\n", msg );
    exit(2);
  }
}

void* TY_MEM(MemAlloc)( size_t size )
{
    void *p = ( g_malloc ? g_malloc(size) : malloc(size) );
    if ( !p )
        FatalError("Out of memory!");
    return p;
}

void* TY_MEM(MemRealloc)( void* mem, size_t newsize )
{
    void *p;
    if ( mem == NULL )
        return TY_MEM(MemAlloc)( newsize );

    p = ( g_realloc ? g_realloc(mem, newsize) : realloc(mem, newsize) );
    if (!p)
        FatalError("Out of memory!");
    return p;
}

void TY_MEM(MemFree)( void* mem )
{
    if ( mem )
    {
        if ( g_free )
            g_free( mem );
        else
            free( mem );
    }
}

void ClearMemory( void *mem, size_t size )
{
    memset(mem, 0, size);
}

bool linkedOnOrAfter2024EReleases(void)
{
#ifdef TIDY_LINKED_ON_OR_AFTER_MACOS15_4_IOS18_4_TVOS18_4_VISIONOS2_4_WATCHOS11_4
    static bool result;
    static dispatch_once_t once;
    dispatch_once(&once, ^{
        result = dyld_program_minos_at_least(dyld_2024_SU_E_os_versions);
    });
    return result;
#else
    return false;
#endif
}
