#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <type_traits>

#include "../lib/wire.hpp"

template <class WireDirection>
class _test_WireCRTP {
private:
	static_assert(std::is_same_v<WireDirection, InWire> || std::is_same_v<WireDirection, OutWire>);
	using WireOppositeDirection = std::conditional_t<std::is_same_v<WireDirection, InWire>, OutWire, InWire>;
public:

	decltype(auto) ToBasicTest() 
	{
		// Operation from basic test
		{
		Basic_Wire b1 {{1.f, -2.f}};
		WireDirection i {std::pair{1.f, -2.f}};

		EXPECT_THAT(b1.getuv(), ::testing::Eq(std::pair(1.f, -2.f)));
		EXPECT_THAT(i.getuv(), ::testing::Eq(std::pair(1.f, -2.f)));
		EXPECT_THAT(i.is_tethered(), ::testing::IsFalse());
		}
	}

	decltype(auto) ConstructionAndAssignmentFromAnotherWireTest() {
		// Operations with opposite direction test
		{
		const constexpr std::pair uvpair{1.f, -.5f};
		WireOppositeDirection o = std::pair{3.f, -2.f};

		WireDirection i1 = uvpair;
		auto res = (i1 >> o);
		if (!res)
			FAIL_AT(__FILE__, __LINE__ - 2lu);

		ASSERT_THAT(i1.is_tethered(), ::testing::IsTrue());
		ASSERT_THAT(o.is_tethered(), ::testing::IsTrue());
		ASSERT_THAT(i1.tethered_view(), ::testing::Ref(o));
		ASSERT_THAT(o.tethered_view(), ::testing::Ref(i1));
		
		WireDirection i2 {std::move(i1)};
		ASSERT_THAT(i2.getuv(), ::testing::Eq(uvpair));

		ASSERT_THAT(i2.is_tethered(), ::testing::IsTrue());
		ASSERT_THAT(o.is_tethered(), ::testing::IsTrue());
		ASSERT_THAT(i2.tethered_view(), ::testing::Ref(o));
		ASSERT_THAT(o.tethered_view(), ::testing::Ref(i2));

		WireDirection i3 {i2};
		ASSERT_THAT(i3.getuv(), ::testing::Eq(i2.getuv()));

		ASSERT_THAT(i3.is_tethered(), ::testing::IsTrue());
		ASSERT_THAT(o.is_tethered(), ::testing::IsTrue());
		ASSERT_THAT(i3.tethered_view(), ::testing::Ref(o));
		ASSERT_THAT(o.tethered_view(), ::testing::Ref(i3));

		WireDirection i4 = std::move(i3);
		ASSERT_THAT(i4.getuv(), ::testing::Eq(i2.getuv()));
	
		ASSERT_THAT(i4.is_tethered(), ::testing::IsTrue());
		ASSERT_THAT(o.is_tethered(), ::testing::IsTrue());
		ASSERT_THAT(i4.tethered_view(), ::testing::Ref(o));
		ASSERT_THAT(o.tethered_view(), ::testing::Ref(i4));

		WireDirection i5 = i4;
		ASSERT_THAT(i5.getuv(), ::testing::Eq(i4.getuv()));

		ASSERT_THAT(i5.is_tethered(), ::testing::IsTrue());
		ASSERT_THAT(o.is_tethered(), ::testing::IsTrue());
		ASSERT_THAT(i5.tethered_view(), ::testing::Ref(o));
		ASSERT_THAT(o.tethered_view(), ::testing::Ref(i5));
		}
	}

	decltype(auto) AXISPacketTest() {
		WireDirection w {std::pair{4.f, -5.f}};
		AXIPacket<WireDirection, WireOppositeDirection> axisp {w, std::pair{1.f, -2.f}};

		ASSERT_THAT(axisp.get_slub().getuv(), ::testing::Eq(std::pair{1.f, -2.f}));
		ASSERT_THAT(axisp.get_slub().is_tethered(), ::testing::IsTrue());
		ASSERT_THAT(axisp.get_slub().tethered_view(), ::testing::Ref(w));
		
		{
		AXIPacket<WireDirection, WireOppositeDirection> axisp2c {axisp};
		AXIPacket<WireDirection, WireOppositeDirection> axisp2ac = axisp;

		auto axisp2c1 {axisp};
		auto axisp2c2 {axisp};
		AXIPacket<WireDirection, WireOppositeDirection> axisp2m {std::move(axisp2c1)};
		AXIPacket<WireDirection, WireOppositeDirection> axisp2am = std::move(axisp2c2);

		for(const auto& it : {axisp2c, axisp2ac, axisp2m, axisp2am}) {
			ASSERT_THAT(it.get_slub().getuv(), ::testing::Eq(axisp.get_slub().getuv()));
			ASSERT_THAT(it.get_slub().is_tethered(), ::testing::IsTrue());
			ASSERT_THAT(it.get_slub().tethered_view(), ::testing::Ref(w));
		}
		}

		auto axisp2c1 {axisp};
		auto axisp2c2 {axisp};
		WireOppositeDirection owm {std::move(axisp2c1)};
		WireOppositeDirection owam = std::move(axisp2c2);

		for(const auto& it : {owm, owam}) {
			ASSERT_THAT(it.getuv(), ::testing::Eq(std::pair{1.f, -2.f}));
			ASSERT_THAT(it.is_tethered(), ::testing::IsTrue());
			ASSERT_THAT(it.tethered_view(), ::testing::Ref(w));
			ASSERT_THAT(w.is_tethered(), ::testing::IsTrue());
			ASSERT_THAT(w.tethered_view(), ::testing::Ref(it));
		}
	}
	
	decltype(auto) MakeTetheredTest() {
		// Make tethered test
		{
		InWire i {std::pair{4.f, -3.5f}};
		OutWire o = i.make_tethered(std::pair{1.5f, -0.5f});

		ASSERT_THAT(o.is_tethered(), ::testing::IsTrue);
		ASSERT_THAT(i.is_tethered(), ::testing::IsTrue);
		EXPECT_THAT(i.tethered_view(), ::testing::Ref(o));
		EXPECT_THAT(o.tethered_view(), ::testing::Ref(i));
		}
	}
};

class _test_InWire : public _test_WireCRTP<InWire> {};
class _test_OutWire : public _test_WireCRTP<OutWire> {};

TEST(CorrectWireConstructionFromBasic, InWire) {
	return _test_InWire{}.ToBasicTest();
}
TEST(CorrectWireConstructionFromBasic, OutWire) {
	return _test_OutWire{}.ToBasicTest();
}


TEST(CorrectWireConstructionAndAssignmentFromOtherDirection, InWire) {
	return _test_InWire{}.ConstructionAndAssignmentFromAnotherWireTest();
}
TEST(CorrectWireConstructionAndAssignmentFromOtherDirection, OutWire) {
	return _test_OutWire{}.ConstructionAndAssignmentFromAnotherWireTest();
}


TEST(CorrectMakeTetheredBahaviour, InWire) {
	return _test_InWire{}.MakeTetheredTest();
}
TEST(CorrectMakeTetheredBahaviour, OutWire) {
	return _test_OutWire{}.MakeTetheredTest();
}

TEST(AxisPacketClassTest, Helper) {
	return _test_InWire{}.AXISPacketTest();
	return _test_OutWire{}.AXISPacketTest();
}

TEST(MakeTetheredTest, Helper) {
	return _test_InWire{}.MakeTetheredTest();
	return _test_OutWire{}.MakeTetheredTest();
}
