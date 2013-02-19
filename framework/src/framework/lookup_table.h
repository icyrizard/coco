/*
 * $Id: lookup_table.h 377 2011-02-28 13:09:12Z grelck $
 */


/*
 *
 *  Look-Up-Table (LUT) for Pointers and Strings
 *  --------------------------------------------
 *
 *  Each entry of a LUT can hold a pair [old, new] where 'old' is either a
 *  pointer (void*) or a string (char*) and 'new' is always a pointer (void*).
 *
 *  To insert a pair [old, new] into the LUT where 'old' represents a pointer
 *  or a string, use the function  InsertIntoLUT_P( lut, old, new)  or
 *  InsertIntoLUT_S( lut, old, new)  respectively.
 *  Note here, that it is possible to put doubles (pairs with identical
 *  compare-data) into the LUT, e.g. pairs (a,b), (a,c).
 *  If doubles are not welcome, it may be better to use the function
 *  UpdateLUT_P() instead.
 *
 *  The function  UpdateLUT_?( lut, old, new, &act_new)  checks whether there
 *  is already a pair [old, act_new] in the LUT. If so, 'act_new' is replaced
 *  by 'new', otherwise  InsertIntoLUT_?()  is called to insert the pair into
 *  the LUT.
 *
 *  The function  SearchInLUT_?( lut, old)  searches the LUT for an entry
 *  [old, new]. If the LUT contains such an entry, a pointer to the associated
 *  data 'new' is returned. Otherwise the return value equals NULL.
 *  Note here, that always the *first* matching entry is returned. If the LUT
 *  contains multiple matching entries, use  SearchInLUT_Next?()  to get
 *  the next match.
 *  For example, let the LUT contain pairs (a,b) and (a,c). When looking-up 'a'
 *  now, the return value is 'b' for the first, 'c' for the second, and NULL
 *  for all subsequent look-ups.
 *  SearchInLUT_P()  searches for a pointer (pointer compare),
 *  SearchInLUT_S()  searches for a string (string compare).
 *
 *  If both values of the pairs [old, new] have the same type (both strings or
 *  both pointers), you may want to use the function  SearchInLUT_SS( lut, old)
 *  or  SearchInLUT_PP( lut, old)  instead.
 *  If the LUT contains an entry [old, new] the associated data 'new' itself
 *  is returned (not a pointer to it). Otherwise the return value equals 'old'
 *  (not NULL).
 *  SearchInLUT_PP()  searches for a pointer (pointer compare) and expects
 *                    to find a pointer,
 *  SearchInLUT_SS()  searches for a string (string compare) and expects
 *                    to find a string.
 *
 *  You can map a function 'fun' to all associated data present in the LUT
 *  by means of  MapLUT( lut, fun). Let  ass_t  be the type of the associated
 *  data in the LUT, then 'fun' should have the signature  ass_t -> ass_t  .
 *  Moreover, the associated data can be folded by using the function
 *  FoldLUT( lut, init, fun)  , where 'init' is the initial value for the
 *  fold operation. Let again  ass_t  be the type of the associated data in
 *  the LUT and let  init_t  be the type of 'init'. Then 'fun' should have
 *  the signature  ( init_t , ass_t ) -> init_t  and the return value of
 *  FoldLUT()  has the type  init_t  .
 *
 *  *** CAUTION ***
 *  - InsertIntoLUT_S()  copies the compare-string ('old') before inserting
 *    it into the LUT. But the associated data is never copied!!
 *    If the associated data is also a string, you may want to duplicate it
 *    with  StringCopy()  first.
 *  - RemoveLUT()  removes all the stored compare-strings from heap memory.
 *  - SearchInLUT_?()  returns a *pointer* to the found associated data. Thus,
 *    the returned pointer will be undefined if  RemoveLUT()  has been called.
 *    Therefore you should not forget to duplicate the data first ... :-/
 *  - The support for multiple entries with identical compare-data is
 *    implemented rather inefficiently :-(
 *
 */


#ifndef _SAC_LOOKUP_TABLE_H_
#define _SAC_LOOKUP_TABLE_H_

#include "types.h"

typedef struct LUT_T lut_t;

extern lut_t *LUTgenerateLut ( void);
extern lut_t *LUTduplicateLut( lut_t *lut);
extern lut_t *LUTremoveContentLut( lut_t *lut);
extern lut_t *LUTremoveLut   ( lut_t *lut);
extern void   LUTtouchContentLut( lut_t *lut, info *arg_info);
extern void   LUTtouchLut( lut_t *lut, info *arg_info);

extern bool   LUTisEmptyLut( lut_t *lut);

extern void **LUTsearchInLutP( lut_t *lut, void *old_item);
extern void **LUTsearchInLutS( lut_t *lut, char *old_item);

extern void **LUTsearchInLutNextP( void);
extern void **LUTsearchInLutNextS( void);

extern void  *LUTsearchInLutPp( lut_t *lut, void *old_item);
extern char  *LUTsearchInLutSs( lut_t *lut, char *old_item);

extern lut_t *LUTinsertIntoLutP( lut_t *lut, void *old_item, void *new_item);
extern lut_t *LUTinsertIntoLutS( lut_t *lut, char *old_item, void *new_item);

extern lut_t *LUTupdateLutP( lut_t *lut, void *old_item, void *new_item,
			     void **found_item);
extern lut_t *LUTupdateLutS( lut_t *lut, char *old_item, void *new_item,
			     void **found_item);

extern lut_t *LUTmapLutS ( lut_t *lut, void *(*fun)( void *));
extern lut_t *LUTmapLutP ( lut_t *lut, void *(*fun)( void *));
extern void  *LUTfoldLutS( lut_t *lut, void *init, 
			   void *(*fun)( void *, void *));
extern void  *LUTfoldLutP( lut_t *lut, void *init, 
			   void *(*fun)( void *, void *));

#endif /* _SAC_LOOKUP_TABLE_H_ */
