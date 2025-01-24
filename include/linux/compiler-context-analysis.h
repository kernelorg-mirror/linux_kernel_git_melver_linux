/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Macros and attributes for compiler-based static context analysis.
 */

#ifndef _LINUX_COMPILER_CONTEXT_ANALYSIS_H
#define _LINUX_COMPILER_CONTEXT_ANALYSIS_H

#if defined(WARN_CONTEXT_ANALYSIS)

/*
 * These attributes define new context guard (Clang: capability) types.
 * Internal only.
 */
# define __ctx_guard_type(name)			__attribute__((capability(#name)))
# define __reentrant_ctx_guard			__attribute__((reentrant_capability))
# define __acquires_ctx_guard(...)		__attribute__((acquire_capability(__VA_ARGS__)))
# define __acquires_shared_ctx_guard(...)	__attribute__((acquire_shared_capability(__VA_ARGS__)))
# define __try_acquires_ctx_guard(ret, var)	__attribute__((try_acquire_capability(ret, var)))
# define __try_acquires_shared_ctx_guard(ret, var) __attribute__((try_acquire_shared_capability(ret, var)))
# define __releases_ctx_guard(...)		__attribute__((release_capability(__VA_ARGS__)))
# define __releases_shared_ctx_guard(...)	__attribute__((release_shared_capability(__VA_ARGS__)))
# define __assumes_ctx_guard(...)		__attribute__((assert_capability(__VA_ARGS__)))
# define __assumes_shared_ctx_guard(...)	__attribute__((assert_shared_capability(__VA_ARGS__)))
# define __returns_ctx_guard(var)		__attribute__((lock_returned(var)))

/*
 * The below are used to annotate code being checked. Internal only.
 */
# define __excludes_ctx_guard(...)		__attribute__((locks_excluded(__VA_ARGS__)))
# define __requires_ctx_guard(...)		__attribute__((requires_capability(__VA_ARGS__)))
# define __requires_shared_ctx_guard(...)	__attribute__((requires_shared_capability(__VA_ARGS__)))

/**
 * __guarded_by - struct member and globals attribute, declares variable
 *                only accessible within active context
 *
 * Declares that the struct member or global variable is only accessible within
 * the context entered by the given context guard. Read operations on the data
 * require shared access, while write operations require exclusive access.
 *
 * .. code-block:: c
 *
 *	struct some_state {
 *		spinlock_t lock;
 *		long counter __guarded_by(&lock);
 *	};
 */
# define __guarded_by(...)		__attribute__((guarded_by(__VA_ARGS__)))

/**
 * __pt_guarded_by - struct member and globals attribute, declares pointed-to
 *                   data only accessible within active context
 *
 * Declares that the data pointed to by the struct member pointer or global
 * pointer is only accessible within the context entered by the given context
 * guard. Read operations on the data require shared access, while write
 * operations require exclusive access.
 *
 * .. code-block:: c
 *
 *	struct some_state {
 *		spinlock_t lock;
 *		long *counter __pt_guarded_by(&lock);
 *	};
 */
# define __pt_guarded_by(...)		__attribute__((pt_guarded_by(__VA_ARGS__)))

/**
 * context_guard_struct() - declare or define a context guard struct
 * @name: struct name
 *
 * Helper to declare or define a struct type that is also a context guard.
 *
 * .. code-block:: c
 *
 *	context_guard_struct(my_handle) {
 *		int foo;
 *		long bar;
 *	};
 *
 *	struct some_state {
 *		...
 *	};
 *	// ... declared elsewhere ...
 *	context_guard_struct(some_state);
 *
 * Note: The implementation defines several helper functions that can acquire
 * and release the context guard.
 */
# define context_guard_struct(name, ...)								\
	struct __ctx_guard_type(name) __VA_ARGS__ name;							\
	static __always_inline void __acquire_ctx_guard(const struct name *var)				\
		__attribute__((overloadable)) __no_context_analysis __acquires_ctx_guard(var) { }	\
	static __always_inline void __acquire_shared_ctx_guard(const struct name *var)			\
		__attribute__((overloadable)) __no_context_analysis __acquires_shared_ctx_guard(var) { } \
	static __always_inline bool __try_acquire_ctx_guard(const struct name *var, bool ret)		\
		__attribute__((overloadable)) __no_context_analysis __try_acquires_ctx_guard(1, var)	\
	{ return ret; }											\
	static __always_inline bool __try_acquire_shared_ctx_guard(const struct name *var, bool ret)	\
		__attribute__((overloadable)) __no_context_analysis __try_acquires_shared_ctx_guard(1, var) \
	{ return ret; }											\
	static __always_inline void __release_ctx_guard(const struct name *var)				\
		__attribute__((overloadable)) __no_context_analysis __releases_ctx_guard(var) { }	\
	static __always_inline void __release_shared_ctx_guard(const struct name *var)			\
		__attribute__((overloadable)) __no_context_analysis __releases_shared_ctx_guard(var) { } \
	static __always_inline void __assume_ctx_guard(const struct name *var)				\
		__attribute__((overloadable)) __assumes_ctx_guard(var) { }				\
	static __always_inline void __assume_shared_ctx_guard(const struct name *var)			\
		__attribute__((overloadable)) __assumes_shared_ctx_guard(var) { }			\
	struct name

/**
 * disable_context_analysis() - disables context analysis
 *
 * Disables context analysis. Must be paired with a later
 * enable_context_analysis().
 */
# define disable_context_analysis()				\
	__diag_push();						\
	__diag_ignore_all("-Wunknown-warning-option", "")	\
	__diag_ignore_all("-Wthread-safety", "")		\
	__diag_ignore_all("-Wthread-safety-pointer", "")

/**
 * enable_context_analysis() - re-enables context analysis
 *
 * Re-enables context analysis. Must be paired with a prior
 * disable_context_analysis().
 */
# define enable_context_analysis() __diag_pop()

/**
 * __no_context_analysis - function attribute, disables context analysis
 *
 * Function attribute denoting that context analysis is disabled for the
 * whole function. Prefer use of `context_unsafe()` where possible.
 */
# define __no_context_analysis	__attribute__((no_thread_safety_analysis))

#else /* !WARN_CONTEXT_ANALYSIS */

# define __ctx_guard_type(name)
# define __reentrant_ctx_guard
# define __acquires_ctx_guard(...)
# define __acquires_shared_ctx_guard(...)
# define __try_acquires_ctx_guard(ret, var)
# define __try_acquires_shared_ctx_guard(ret, var)
# define __releases_ctx_guard(...)
# define __releases_shared_ctx_guard(...)
# define __assumes_ctx_guard(...)
# define __assumes_shared_ctx_guard(...)
# define __returns_ctx_guard(var)
# define __guarded_by(...)
# define __pt_guarded_by(...)
# define __excludes_ctx_guard(...)
# define __requires_ctx_guard(...)
# define __requires_shared_ctx_guard(...)
# define __acquire_ctx_guard(var)			do { } while (0)
# define __acquire_shared_ctx_guard(var)		do { } while (0)
# define __try_acquire_ctx_guard(var, ret)		(ret)
# define __try_acquire_shared_ctx_guard(var, ret)	(ret)
# define __release_ctx_guard(var)			do { } while (0)
# define __release_shared_ctx_guard(var)		do { } while (0)
# define __assume_ctx_guard(var)			do { (void)(var); } while (0)
# define __assume_shared_ctx_guard(var)			do { (void)(var); } while (0)
# define context_guard_struct(name, ...)		struct __VA_ARGS__ name
# define disable_context_analysis()
# define enable_context_analysis()
# define __no_context_analysis

#endif /* WARN_CONTEXT_ANALYSIS */

/**
 * context_unsafe() - disable context checking for contained code
 *
 * Disables context checking for contained statements or expression.
 *
 * .. code-block:: c
 *
 *	struct some_data {
 *		spinlock_t lock;
 *		int counter __guarded_by(&lock);
 *	};
 *
 *	int foo(struct some_data *d)
 *	{
 *		// ...
 *		// other code that is still checked ...
 *		// ...
 *		return context_unsafe(d->counter);
 *	}
 */
#define context_unsafe(...)		\
({					\
	disable_context_analysis();	\
	__VA_ARGS__;			\
	enable_context_analysis()	\
})

/**
 * __context_unsafe() - function attribute, disable context checking
 * @comment: comment explaining why opt-out is safe
 *
 * Function attribute denoting that context analysis is disabled for the
 * whole function. Forces adding an inline comment as argument.
 */
#define __context_unsafe(comment) __no_context_analysis

/**
 * context_unsafe_alias() - helper to insert a context guard "alias barrier"
 * @p: pointer aliasing a context guard or object containing context guards
 *
 * No-op function that acts as a "context guard alias barrier", where the
 * analysis rightfully detects that we're switching aliases, but the switch is
 * considered safe but beyond the analysis reasoning abilities.
 *
 * This should be inserted before the first use of such an alias.
 *
 * Implementation Note: The compiler ignores aliases that may be reassigned but
 * their value cannot be determined (e.g. when passing a non-const pointer to an
 * alias as a function argument).
 */
#define context_unsafe_alias(p) _context_unsafe_alias((void **)&(p))
static inline void _context_unsafe_alias(void **p) { }

/**
 * token_context_guard() - declare an abstract global context guard instance
 * @name: token context guard name
 *
 * Helper that declares an abstract global context guard instance @name, but not
 * backed by a real data structure (linker error if accidentally referenced).
 * The type name is `__ctx_guard_@name`.
 */
#define token_context_guard(name, ...)					\
	context_guard_struct(__ctx_guard_##name, ##__VA_ARGS__) {};	\
	extern const struct __ctx_guard_##name *name

/**
 * token_context_guard_instance() - declare another instance of a global context guard
 * @ctx: token context guard previously declared with token_context_guard()
 * @name: name of additional global context guard instance
 *
 * Helper that declares an additional instance @name of the same token context
 * guard class @ctx. This is helpful where multiple related token contexts are
 * declared, to allow using the same underlying type (`__ctx_guard_@ctx`) as
 * function arguments.
 */
#define token_context_guard_instance(ctx, name)		\
	extern const struct __ctx_guard_##ctx *name

/*
 * Common keywords for static context analysis. Both Clang's "capability
 * analysis" and Sparse's "context tracking" are currently supported.
 */
#ifdef __CHECKER__

/* Sparse context/lock checking support. */
# define __must_hold(x)		__attribute__((context(x,1,1)))
# define __must_not_hold(x)
# define __acquires(x)		__attribute__((context(x,0,1)))
# define __cond_acquires(ret, x) __attribute__((context(x,0,-1)))
# define __releases(x)		__attribute__((context(x,1,0)))
# define __acquire(x)		__context__(x,1)
# define __release(x)		__context__(x,-1)
# define __cond_lock(x, c)	((c) ? ({ __acquire(x); 1; }) : 0)
/* For Sparse, there's no distinction between exclusive and shared locks. */
# define __must_hold_shared	__must_hold
# define __acquires_shared	__acquires
# define __cond_acquires_shared __cond_acquires
# define __releases_shared	__releases
# define __acquire_shared	__acquire
# define __release_shared	__release
# define __cond_lock_shared	__cond_acquire

#else /* !__CHECKER__ */

/**
 * __must_hold() - function attribute, caller must hold exclusive context guard
 * @x: context guard instance pointer
 *
 * Function attribute declaring that the caller must hold the given context
 * guard instance @x exclusively.
 */
# define __must_hold(x)		__requires_ctx_guard(x)

/**
 * __must_not_hold() - function attribute, caller must not hold context guard
 * @x: context guard instance pointer
 *
 * Function attribute declaring that the caller must not hold the given context
 * guard instance @x.
 */
# define __must_not_hold(x)	__excludes_ctx_guard(x)

/**
 * __acquires() - function attribute, function acquires context guard exclusively
 * @x: context guard instance pointer
 *
 * Function attribute declaring that the function acquires the given context
 * guard instance @x exclusively, but does not release it.
 */
# define __acquires(x)		__acquires_ctx_guard(x)

/*
 * Clang's analysis does not care precisely about the value, only that it is
 * either zero or non-zero. So the __cond_acquires() interface might be
 * misleading if we say that @ret is the value returned if acquired. Instead,
 * provide symbolic variants which we translate.
 */
#define __cond_acquires_impl_true(x, ...)     __try_acquires##__VA_ARGS__##_ctx_guard(1, x)
#define __cond_acquires_impl_false(x, ...)    __try_acquires##__VA_ARGS__##_ctx_guard(0, x)
#define __cond_acquires_impl_nonzero(x, ...)  __try_acquires##__VA_ARGS__##_ctx_guard(1, x)
#define __cond_acquires_impl_0(x, ...)        __try_acquires##__VA_ARGS__##_ctx_guard(0, x)
#define __cond_acquires_impl_nonnull(x, ...)  __try_acquires##__VA_ARGS__##_ctx_guard(1, x)
#define __cond_acquires_impl_NULL(x, ...)     __try_acquires##__VA_ARGS__##_ctx_guard(0, x)

/**
 * __cond_acquires() - function attribute, function conditionally
 *                     acquires a context guard exclusively
 * @ret: abstract value returned by function if context guard acquired
 * @x: context guard instance pointer
 *
 * Function attribute declaring that the function conditionally acquires the
 * given context guard instance @x exclusively, but does not release it. The
 * function return value @ret denotes when the context guard is acquired.
 *
 * @ret may be one of: true, false, nonzero, 0, nonnull, NULL.
 */
# define __cond_acquires(ret, x) __cond_acquires_impl_##ret(x)

/**
 * __releases() - function attribute, function releases a context guard exclusively
 * @x: context guard instance pointer
 *
 * Function attribute declaring that the function releases the given context
 * guard instance @x exclusively. The associated context must be active on
 * entry.
 */
# define __releases(x)		__releases_ctx_guard(x)

/**
 * __acquire() - function to acquire context guard exclusively
 * @x: context guard instance pointer
 *
 * No-op function that acquires the given context guard instance @x exclusively.
 */
# define __acquire(x)		__acquire_ctx_guard(x)

/**
 * __release() - function to release context guard exclusively
 * @x: context guard instance pointer
 *
 * No-op function that releases the given context guard instance @x.
 */
# define __release(x)		__release_ctx_guard(x)

/**
 * __cond_lock() - function that conditionally acquires a context guard
 *                 exclusively
 * @x: context guard instance pinter
 * @c: boolean expression
 *
 * Return: result of @c
 *
 * No-op function that conditionally acquires context guard instance @x
 * exclusively, if the boolean expression @c is true. The result of @c is the
 * return value; for example:
 *
 * .. code-block:: c
 *
 *	#define spin_trylock(l) __cond_lock(&lock, _spin_trylock(&lock))
 */
# define __cond_lock(x, c)	__try_acquire_ctx_guard(x, c)

/**
 * __must_hold_shared() - function attribute, caller must hold shared context guard
 * @x: context guard instance pointer
 *
 * Function attribute declaring that the caller must hold the given context
 * guard instance @x with shared access.
 */
# define __must_hold_shared(x)	__requires_shared_ctx_guard(x)

/**
 * __acquires_shared() - function attribute, function acquires context guard shared
 * @x: context guard instance pointer
 *
 * Function attribute declaring that the function acquires the given
 * context guard instance @x with shared access, but does not release it.
 */
# define __acquires_shared(x)	__acquires_shared_ctx_guard(x)

/**
 * __cond_acquires_shared() - function attribute, function conditionally
 *                            acquires a context guard shared
 * @ret: abstract value returned by function if context guard acquired
 * @x: context guard instance pointer
 *
 * Function attribute declaring that the function conditionally acquires the
 * given context guard instance @x with shared access, but does not release it. The
 * function return value @ret denotes when the context guard is acquired.
 *
 * @ret may be one of: true, false, nonzero, 0, nonnull, NULL.
 */
# define __cond_acquires_shared(ret, x) __cond_acquires_impl_##ret(x, _shared)

/**
 * __releases_shared() - function attribute, function releases a
 *                       context guard shared
 * @x: context guard instance pointer
 *
 * Function attribute declaring that the function releases the given context
 * guard instance @x with shared access. The associated context must be active
 * on entry.
 */
# define __releases_shared(x)	__releases_shared_ctx_guard(x)

/**
 * __acquire_shared() - function to acquire context guard shared
 * @x: context guard instance pointer
 *
 * No-op function that acquires the given context guard instance @x with shared
 * access.
 */
# define __acquire_shared(x)	__acquire_shared_ctx_guard(x)

/**
 * __release_shared() - function to release context guard shared
 * @x: context guard instance pointer
 *
 * No-op function that releases the given context guard instance @x with shared
 * access.
 */
# define __release_shared(x)	__release_shared_ctx_guard(x)

/**
 * __cond_lock_shared() - function that conditionally acquires a context guard shared
 * @x: context guard instance pinter
 * @c: boolean expression
 *
 * Return: result of @c
 *
 * No-op function that conditionally acquires context guard instance @x with
 * shared access, if the boolean expression @c is true. The result of @c is the
 * return value.
 */
# define __cond_lock_shared(x, c) __try_acquire_shared_ctx_guard(x, c)

#endif /* __CHECKER__ */

/**
 * __acquire_ret() - helper to acquire context guard of return value
 * @call: call expression
 * @ret_expr: acquire expression that uses __ret
 */
#define __acquire_ret(call, ret_expr)		\
	({					\
		__auto_type __ret = call;	\
		__acquire(ret_expr);		\
		__ret;				\
	})

/**
 * __acquire_shared_ret() - helper to acquire context guard shared of return value
 * @call: call expression
 * @ret_expr: acquire shared expression that uses __ret
 */
#define __acquire_shared_ret(call, ret_expr)	\
	({					\
		__auto_type __ret = call;	\
		__acquire_shared(ret_expr);	\
		__ret;				\
	})

/*
 * Attributes to mark functions returning acquired context guards.
 *
 * This is purely cosmetic to help readability, and should be used with the
 * above macros as follows:
 *
 *   struct foo { spinlock_t lock; ... };
 *   ...
 *   #define myfunc(...) __acquire_ret(_myfunc(__VA_ARGS__), &__ret->lock)
 *   struct foo *_myfunc(int bar) __acquires_ret;
 *   ...
 */
#define __acquires_ret		__no_context_analysis
#define __acquires_shared_ret	__no_context_analysis

#endif /* _LINUX_COMPILER_CONTEXT_ANALYSIS_H */
