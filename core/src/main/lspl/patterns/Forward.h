#ifndef _LSPL_PATTERNS_FORWARD_H_
#define _LSPL_PATTERNS_FORWARD_H_

#include "../base/RefCountObject.h"

#include <vector>

namespace lspl { namespace patterns {

class Alternative;

LSPL_REFCOUNT_FORWARD( Pattern );
LSPL_REFCOUNT_FORWARD( PatternBuilder );
LSPL_REFCOUNT_FORWARD( TextTransformParser );
LSPL_REFCOUNT_FORWARD( PatternTransformParser );

} } // namespace lspl::patterns

#endif /*_LSPL_PATTERNS_FORWARD_H_*/
