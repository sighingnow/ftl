/*
 * Copyright (c) 2013 Björn Aili
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 * claim that you wrote the original software. If you use this software
 * in a product, an acknowledgment in the product documentation would be
 * appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not be
 * misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source
 * distribution.
 */
#ifndef FTL_FUNCTIONAL_H
#define FTL_FUNCTIONAL_H

#include "function.h"
#include "monoid.h"

namespace ftl {

	/**
	 * Monoid instance for ftl::functions returning monoids.
	 *
	 * The reason this works might not be immediately obvious, but basically,
	 * any function (regardless of arity) that returns a value that is an
	 * instance of monoid, is in fact also a monoid. It works as follows:
	 * \code
	 *   id() <=> A function, returning monoid<result_type>::id(),
	 *            regardless of parameters.
	 *   append(a,b) <=> A function that forwards its arguments to both a and b,
	 *                   and then calls monoid<result_type>::append on the two
	 *                   results.
	 * \endcode
	 */
	template<typename M, typename...Ps>
	struct monoid<function<M,Ps...>> {
		static function<M,Ps...> id() {
			return [](Ps...ps) { return monoid<M>::id(); };
		}

		static function<M,Ps...> append(
				const function<M,Ps...>& f1,
				const function<M,Ps...>& f2) {
			return [=] (Ps...ps) {
				return monoid<M>::append(f1(ps...), f2(ps...));
			};
		}
	};

	template<typename M, typename...Ps>
	function<M,Ps...> operator^ (
			const function<M,Ps...>& f1,
			const function<M,Ps...>& f2) {
		return monoid<function<M,Ps...>>::append(f1, f2);
	}

	template<
		typename F,
		typename A,
		typename B = typename std::result_of<F(A)>::type,
		typename...Ps>
	function<B,Ps...> fmap(F f, function<A,Ps...> fn) {
		return [f,fn] (Ps...ps) {
			return f(fn(std::forward<Ps>(ps)...));
		};
	}

	/*
	 * N-ary curry, commented out until GCC fixes the bug where template
	 * parameter packs cannot be captured in lambds...
	 * TODO: Enable once gcc has fixed this
	namespace {

		template<
			typename R,
			typename T,
			typename...Ts,
			typename...Ps>
		auto curry_rec(function<R,Ts...> f, type_vec<T> dummy, Ps...ps)
		-> function<R,T> {
			return [f,ps...] (T t) {
				return f(std::forward<T>(t), std::forward<Ps>(ps)...);
			};
		}

		template<
			typename R,
			typename T,
			typename...OTs,
			typename...Ts,
			typename...Ps>
		auto curry_rec(function<R,OTs..> f, type_vec<T,Ts...> dummy, Ps...ps)
		-> function<decltype(curry_rec(f,type_vec<Ts...>(),std::forward<T>(T()),std::forward<Ps>(ps)...)),T> {
			return  [f,ps...] (T t) {
				return curry_rec(f, type_vec<Ts...>(), std::forward<T>(t), std::forward<Ps>(ps)...);
			};
		}
	}

	template<
		typename R,
		typename T,
		typename...Ts>
	auto curry(function<R,T,Ts...> f)
	-> function<decltype(curry_rec(f,type_vec<Ts...>(), std::forward<T>(T()))), T> {
		return [f] (T t) {
			return curry_rec(f, type_vec<Ts...>(), std::forward<T>(t));
		};
	}
	*/

	/**
	 * Curries a binary function.
	 *
	 * Currying is the process of turning a function of (a,b) -> c into
	 * a -> b -> c. In other words, instead of taking two arguments and
	 * returning the answer, the curried function takes one argument and
	 * returns a function that takes another one and \em then returns the
	 * answer.
	 */
	template<typename R, typename T1, typename T2>
	function<function<R,T2>,T1> curry(function<R,T1,T2> f) {
		return [f] (T1 t1) {
			return [f,&t1] (T2 t2) {
				return f(std::forward(t1), std::forward(t2));
			};
		}
	}

	/**
	 * Uncurries a binary function.
	 */
	template<typename R, typename T1, typename T2>
	function<R,T1,T2> uncurry(function<function<R,T2>,T1> f) {
		return [f] (T1 t1, T2 t2) {
			return f(std::forward<T1>(t1))(std::forward<T2>(t2);
		}
	}

	/**
	 * Monoid instance for std::functions returning monoids.
	 *
	 * In essence, the same as ftl::function's implementation.
	 */
	template<typename M, typename...Ps>
	struct monoid<std::function<M(Ps...)>> {
		static std::function<M(Ps...)> id() {
			return [](Ps...ps) { return monoid<M>::id(); };
		}

		static std::function<M(Ps...)> append(
				const std::function<M(Ps...)>& f1,
				const std::function<M(Ps...)>& f2) {
			return [=] (Ps...ps) {
				return monoid<M>::append(f1(ps...), f2(ps...));
			};
		}
	};

	template<typename M, typename...Ps>
	std::function<M(Ps...)> operator^ (
			const std::function<M(Ps...)>& f1,
			const std::function<M(Ps...)>& f2) {
		return monoid<std::function<M(Ps...)>>::append(f1, f2);
	}

}

#endif

