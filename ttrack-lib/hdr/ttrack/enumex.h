#include <stdio.h>
#define enumex_eval_end(x) x,
#define enumex_eval_t(x)

#define enumex_sep0_0(a, b, el, ...) enumex_eval_ ## el (a)
#define enumex_sep0_1(a, b, el, ...) enumex_eval_ ## el (a) enumex_sep0_0(__VA_ARGS__, t, t, t)
#define enumex_sep0_2(a, b, el, ...) enumex_eval_ ## el (a) enumex_sep0_1(__VA_ARGS__, t, t, t)
#define enumex_sep0_3(a, b, el, ...) enumex_eval_ ## el (a) enumex_sep0_2(__VA_ARGS__, t, t, t)
#define enumex_sep0_4(a, b, el, ...) enumex_eval_ ## el (a) enumex_sep0_3(__VA_ARGS__, t, t, t)
#define enumex_sep0_5(a, b, el, ...) enumex_eval_ ## el (a) enumex_sep0_4(__VA_ARGS__, t, t, t)
#define enumex_sep0_6(a, b, el, ...) enumex_eval_ ## el (a) enumex_sep0_5(__VA_ARGS__, t, t, t)
#define enumex_sep0_7(a, b, el, ...) enumex_eval_ ## el (a) enumex_sep0_6(__VA_ARGS__, t, t, t)
#define enumex_sep0_8(a, b, el, ...) enumex_eval_ ## el (a) enumex_sep0_7(__VA_ARGS__, t, t, t)
#define enumex_sep0_9(a, b, el, ...) enumex_eval_ ## el (a) enumex_sep0_8(__VA_ARGS__, t, t, t)
#define enumex_sep0_10(a, b, el, ...) enumex_eval_ ## el (a) enumex_sep0_9(__VA_ARGS__, t, t, t)
#define enumex_sep0_11(a, b, el, ...) enumex_eval_ ## el (a) enumex_sep0_10(__VA_ARGS__, t, t, t)
#define enumex_sep0_12(a, b, el, ...) enumex_eval_ ## el (a) enumex_sep0_11(__VA_ARGS__, t, t, t)
#define enumex_sep0_13(a, b, el, ...) enumex_eval_ ## el (a) enumex_sep0_12(__VA_ARGS__, t, t, t)
#define enumex_sep0_14(a, b, el, ...) enumex_eval_ ## el (a) enumex_sep0_13(__VA_ARGS__, t, t, t)
#define enumex_sep0_15(a, b, el, ...) enumex_eval_ ## el (a) enumex_sep0_14(__VA_ARGS__, t, t, t)
#define enumex_sep0_16(a, b, el, ...) enumex_eval_ ## el (a) enumex_sep0_15(__VA_ARGS__, t, t, t)
#define enumex_sep0_17(a, b, el, ...) enumex_eval_ ## el (a) enumex_sep0_16(__VA_ARGS__, t, t, t)
#define enumex_sep0_18(a, b, el, ...) enumex_eval_ ## el (a) enumex_sep0_17(__VA_ARGS__, t, t, t)
#define enumex_sep0_19(a, b, el, ...) enumex_eval_ ## el (a) enumex_sep0_18(__VA_ARGS__, t, t, t)
#define enumex_sep0_20(a, b, el, ...) enumex_eval_ ## el (a) enumex_sep0_19(__VA_ARGS__, t, t, t)
#define enumex_sep0_21(a, b, el, ...) enumex_eval_ ## el (a) enumex_sep0_20(__VA_ARGS__, t, t, t)
#define enumex_sep0_22(a, b, el, ...) enumex_eval_ ## el (a) enumex_sep0_21(__VA_ARGS__, t, t, t)
#define enumex_sep0_23(a, b, el, ...) enumex_eval_ ## el (a) enumex_sep0_22(__VA_ARGS__, t, t, t)
#define enumex_sep0_24(a, b, el, ...) enumex_eval_ ## el (a) enumex_sep0_23(__VA_ARGS__, t, t, t)
#define enumex_sep0_25(a, b, el, ...) enumex_eval_ ## el (a) enumex_sep0_24(__VA_ARGS__, t, t, t)
#define enumex_sep0_26(a, b, el, ...) enumex_eval_ ## el (a) enumex_sep0_25(__VA_ARGS__, t, t, t)
#define enumex_sep0_27(a, b, el, ...) enumex_eval_ ## el (a) enumex_sep0_26(__VA_ARGS__, t, t, t)
#define enumex_sep0_28(a, b, el, ...) enumex_eval_ ## el (a) enumex_sep0_27(__VA_ARGS__, t, t, t)
#define enumex_sep0_29(a, b, el, ...) enumex_eval_ ## el (a) enumex_sep0_28(__VA_ARGS__, t, t, t)
#define enumex_sep0_30(a, b, el, ...) enumex_eval_ ## el (a) enumex_sep0_29(__VA_ARGS__, t, t, t)
#define enumex_sep0_31(a, b, el, ...) enumex_eval_ ## el (a) enumex_sep0_30(__VA_ARGS__, t, t, t)
#define enumex_sep0_32(a, b, el, ...) enumex_eval_ ## el (a) enumex_sep0_31(__VA_ARGS__, t, t, t)
#define enumex_sep0_33(a, b, el, ...) enumex_eval_ ## el (a) enumex_sep0_32(__VA_ARGS__, t, t, t)
#define enumex_sep0_34(a, b, el, ...) enumex_eval_ ## el (a) enumex_sep0_33(__VA_ARGS__, t, t, t)
#define enumex_sep0_35(a, b, el, ...) enumex_eval_ ## el (a) enumex_sep0_34(__VA_ARGS__, t, t, t)
#define enumex_sep0_36(a, b, el, ...) enumex_eval_ ## el (a) enumex_sep0_35(__VA_ARGS__, t, t, t)
#define enumex_sep0_37(a, b, el, ...) enumex_eval_ ## el (a) enumex_sep0_36(__VA_ARGS__, t, t, t)
#define enumex_sep0_38(a, b, el, ...) enumex_eval_ ## el (a) enumex_sep0_37(__VA_ARGS__, t, t, t)
#define enumex_sep0_39(a, b, el, ...) enumex_eval_ ## el (a) enumex_sep0_38(__VA_ARGS__, t, t, t)
#define enumex_sep0_40(a, b, el, ...) enumex_eval_ ## el (a) enumex_sep0_39(__VA_ARGS__, t, t, t)
#define enumex_sep1_0(a, b, el, ...) enumex_eval_ ## el (b)
#define enumex_sep1_1(a, b, el, ...) enumex_eval_ ## el (b) enumex_sep1_0(__VA_ARGS__, t, t, t)
#define enumex_sep1_2(a, b, el, ...) enumex_eval_ ## el (b) enumex_sep1_1(__VA_ARGS__, t, t, t)
#define enumex_sep1_3(a, b, el, ...) enumex_eval_ ## el (b) enumex_sep1_2(__VA_ARGS__, t, t, t)
#define enumex_sep1_4(a, b, el, ...) enumex_eval_ ## el (b) enumex_sep1_3(__VA_ARGS__, t, t, t)
#define enumex_sep1_5(a, b, el, ...) enumex_eval_ ## el (b) enumex_sep1_4(__VA_ARGS__, t, t, t)
#define enumex_sep1_6(a, b, el, ...) enumex_eval_ ## el (b) enumex_sep1_5(__VA_ARGS__, t, t, t)
#define enumex_sep1_7(a, b, el, ...) enumex_eval_ ## el (b) enumex_sep1_6(__VA_ARGS__, t, t, t)
#define enumex_sep1_8(a, b, el, ...) enumex_eval_ ## el (b) enumex_sep1_7(__VA_ARGS__, t, t, t)
#define enumex_sep1_9(a, b, el, ...) enumex_eval_ ## el (b) enumex_sep1_8(__VA_ARGS__, t, t, t)
#define enumex_sep1_10(a, b, el, ...) enumex_eval_ ## el (b) enumex_sep1_9(__VA_ARGS__, t, t, t)
#define enumex_sep1_11(a, b, el, ...) enumex_eval_ ## el (b) enumex_sep1_10(__VA_ARGS__, t, t, t)
#define enumex_sep1_12(a, b, el, ...) enumex_eval_ ## el (b) enumex_sep1_11(__VA_ARGS__, t, t, t)
#define enumex_sep1_13(a, b, el, ...) enumex_eval_ ## el (b) enumex_sep1_12(__VA_ARGS__, t, t, t)
#define enumex_sep1_14(a, b, el, ...) enumex_eval_ ## el (b) enumex_sep1_13(__VA_ARGS__, t, t, t)
#define enumex_sep1_15(a, b, el, ...) enumex_eval_ ## el (b) enumex_sep1_14(__VA_ARGS__, t, t, t)
#define enumex_sep1_16(a, b, el, ...) enumex_eval_ ## el (b) enumex_sep1_15(__VA_ARGS__, t, t, t)
#define enumex_sep1_17(a, b, el, ...) enumex_eval_ ## el (b) enumex_sep1_16(__VA_ARGS__, t, t, t)
#define enumex_sep1_18(a, b, el, ...) enumex_eval_ ## el (b) enumex_sep1_17(__VA_ARGS__, t, t, t)
#define enumex_sep1_19(a, b, el, ...) enumex_eval_ ## el (b) enumex_sep1_18(__VA_ARGS__, t, t, t)
#define enumex_sep1_20(a, b, el, ...) enumex_eval_ ## el (b) enumex_sep1_19(__VA_ARGS__, t, t, t)
#define enumex_sep1_21(a, b, el, ...) enumex_eval_ ## el (b) enumex_sep1_20(__VA_ARGS__, t, t, t)
#define enumex_sep1_22(a, b, el, ...) enumex_eval_ ## el (b) enumex_sep1_21(__VA_ARGS__, t, t, t)
#define enumex_sep1_23(a, b, el, ...) enumex_eval_ ## el (b) enumex_sep1_22(__VA_ARGS__, t, t, t)
#define enumex_sep1_24(a, b, el, ...) enumex_eval_ ## el (b) enumex_sep1_23(__VA_ARGS__, t, t, t)
#define enumex_sep1_25(a, b, el, ...) enumex_eval_ ## el (b) enumex_sep1_24(__VA_ARGS__, t, t, t)
#define enumex_sep1_26(a, b, el, ...) enumex_eval_ ## el (b) enumex_sep1_25(__VA_ARGS__, t, t, t)
#define enumex_sep1_27(a, b, el, ...) enumex_eval_ ## el (b) enumex_sep1_26(__VA_ARGS__, t, t, t)
#define enumex_sep1_28(a, b, el, ...) enumex_eval_ ## el (b) enumex_sep1_27(__VA_ARGS__, t, t, t)
#define enumex_sep1_29(a, b, el, ...) enumex_eval_ ## el (b) enumex_sep1_28(__VA_ARGS__, t, t, t)
#define enumex_sep1_30(a, b, el, ...) enumex_eval_ ## el (b) enumex_sep1_29(__VA_ARGS__, t, t, t)
#define enumex_sep1_31(a, b, el, ...) enumex_eval_ ## el (b) enumex_sep1_30(__VA_ARGS__, t, t, t)
#define enumex_sep1_32(a, b, el, ...) enumex_eval_ ## el (b) enumex_sep1_31(__VA_ARGS__, t, t, t)
#define enumex_sep1_33(a, b, el, ...) enumex_eval_ ## el (b) enumex_sep1_32(__VA_ARGS__, t, t, t)
#define enumex_sep1_34(a, b, el, ...) enumex_eval_ ## el (b) enumex_sep1_33(__VA_ARGS__, t, t, t)
#define enumex_sep1_35(a, b, el, ...) enumex_eval_ ## el (b) enumex_sep1_34(__VA_ARGS__, t, t, t)
#define enumex_sep1_36(a, b, el, ...) enumex_eval_ ## el (b) enumex_sep1_35(__VA_ARGS__, t, t, t)
#define enumex_sep1_37(a, b, el, ...) enumex_eval_ ## el (b) enumex_sep1_36(__VA_ARGS__, t, t, t)
#define enumex_sep1_38(a, b, el, ...) enumex_eval_ ## el (b) enumex_sep1_37(__VA_ARGS__, t, t, t)
#define enumex_sep1_39(a, b, el, ...) enumex_eval_ ## el (b) enumex_sep1_38(__VA_ARGS__, t, t, t)
#define enumex_sep1_40(a, b, el, ...) enumex_eval_ ## el (b) enumex_sep1_39(__VA_ARGS__, t, t, t)

#define enumex(basename, ...)\
	typedef enum {\
		enumex_sep0_40(__VA_ARGS__, t, t, t)\
	} basename ## _t;\
	char const* basename ## _str(basename ## _t errc) {\
		char const* TABLE[] = { enumex_sep1_40(__VA_ARGS__, t, t, t)  };\
		return TABLE[errc];\
	}