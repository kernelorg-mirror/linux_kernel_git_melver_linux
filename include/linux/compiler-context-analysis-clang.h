/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Macros and attributes for compiler-based static context analysis that map to
 * Clang's "Thread Safety Analysis".
 */

#ifndef _LINUX_COMPILER_CONTEXT_ANALYSIS_CLANG_H
#define _LINUX_COMPILER_CONTEXT_ANALYSIS_CLANG_H

#ifndef WARN_CONTEXT_ANALYSIS
#error "This header should not be included"
#endif

/*
 * These attributes define new context guard (Clang: capability) types.
 * Internal only.
 */
#define __ctx_guard_type(name)			__attribute__((capability(#name)))
#define __reentrant_ctx_guard			__attribute__((reentrant_capability))
#define __acquires_ctx_guard(...)		__attribute__((acquire_capability(__VA_ARGS__)))
#define __acquires_shared_ctx_guard(...)	__attribute__((acquire_shared_capability(__VA_ARGS__)))
#define __try_acquires_ctx_guard(ret, var)	__attribute__((try_acquire_capability(ret, var)))
#define __try_acquires_shared_ctx_guard(ret, var) __attribute__((try_acquire_shared_capability(ret, var)))
#define __releases_ctx_guard(...)		__attribute__((release_capability(__VA_ARGS__)))
#define __releases_shared_ctx_guard(...)	__attribute__((release_shared_capability(__VA_ARGS__)))
#define __assumes_ctx_guard(...)		__attribute__((assert_capability(__VA_ARGS__)))
#define __assumes_shared_ctx_guard(...)	__attribute__((assert_shared_capability(__VA_ARGS__)))
#define __returns_ctx_guard(var)		__attribute__((lock_returned(var)))

/*
 * The below are used to annotate code being checked. Internal only.
 */
#define __excludes_ctx_guard(...)		__attribute__((locks_excluded(__VA_ARGS__)))
#define __requires_ctx_guard(...)		__attribute__((requires_capability(__VA_ARGS__)))
#define __requires_shared_ctx_guard(...)	__attribute__((requires_shared_capability(__VA_ARGS__)))

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
#define __guarded_by(...)		__attribute__((guarded_by(__VA_ARGS__)))

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
#define __pt_guarded_by(...)		__attribute__((pt_guarded_by(__VA_ARGS__)))

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
#define context_guard_struct(name, ...)								\
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
#define disable_context_analysis()				\
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
#define enable_context_analysis() __diag_pop()

/**
 * __no_context_analysis - function attribute, disables context analysis
 *
 * Function attribute denoting that context analysis is disabled for the
 * whole function. Prefer use of `context_unsafe()` where possible.
 */
#define __no_context_analysis	__attribute__((no_thread_safety_analysis))

#endif /* _LINUX_COMPILER_CONTEXT_ANALYSIS_CLANG_H */
