

#ifndef _OPENEDGE_CORE_VERSION_HPP_
#define _OPENEDGE_CORE_VERSION_HPP_

#ifdef __MAJOR__
#define _OE_MAJOR_	__MAJOR__
#else
#define _OE_MAJOR_ 0
#endif

#ifdef __MINOR__
#define _OE_MINOR_	__MINOR__
#else
#define _OE_MINOR_	0
#endif

#ifdef __REV__
#define _OE_REV_	__REV__
#else
#define _OE_REV_	0
#endif


#define _VER_STR_(x) #x
#define _OE_VER_SET_(major, minor, rev)	_VER_STR_(major) "." _VER_STR_(minor) "." _VER_STR_(rev)
#define _OE_VER_	_OE_VER_SET_(_OE_MAJOR_, _OE_MINOR_, _OE_REV_)


#endif