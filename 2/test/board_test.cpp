#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <utility>

#include "../lib/board.hpp"

TEST(Construction, Board) {
	using namespace std::string_view_literals;
	// Construct from one wire
	InWire i1 {std::pair{1.f, -4.f}};
	OutWire o1 {std::pair{4.f, -2.f}};
	//std::variant<InWire, OutWire> i1w{i1}, o1w{o1};
	if (!(i1 >> o1))
		throw "Failed to connect an InWire to an OutWire"sv;

	// Range move/copy correctness

	//std::vector<std::variant<InWire, OutWire>> vc {};
	//vc.push_back(i1w);
	circuit::Board 
		bc {std::vector<std::variant<InWire, OutWire>>{i1}};
	auto v = std::vector<std::variant<InWire, OutWire>>{std::variant<InWire, OutWire>{i1}};
	circuit::Board
		bm {std::move(v)};

	std::vector<circuit::Board> vb{bc, bm};
	for(auto& it : vb) {
		ASSERT_THAT(it.vec().size(), ::testing::Eq(1uz));
		EXPECT_THAT(std::get<InWire>(it[0uz]).getuv(), ::testing::Eq(i1.getuv()));
		ASSERT_THAT(std::get<InWire>(it[0uz]).is_tethered(), ::testing::IsTrue());
		EXPECT_THAT(std::get<InWire>(it[0uz]).tethered_view().value().get(), ::testing::Ref(o1));
		ASSERT_THAT(o1.is_tethered(), ::testing::IsTrue());
		EXPECT_THAT(o1.tethered_view().value().get(), ::testing::Ref(std::get<InWire>(it[0uz])));
	}
	/*	ASSERT_THAT(bc.vec().size(), ::testing::Eq(1uz));
		EXPECT_THAT(std::get<InWire>(bc[0uz]).getuv(), ::testing::Eq(i1.getuv()));
		ASSERT_THAT(std::get<InWire>(bc[0uz]).is_tethered(), ::testing::IsTrue());
		EXPECT_THAT(std::get<InWire>(bc[0uz]).tethered_view().value().get(), ::testing::Ref(o1));
		ASSERT_THAT(o1.is_tethered(), ::testing::IsTrue());
		EXPECT_THAT(o1.tethered_view().value().get(), ::testing::Ref(std::get<InWire>(bc[0uz])));
	}*/
}

TEST(Extend, Board) {
	InWire i1 {std::pair{1.f, -4.f}};
	OutWire o1 {std::pair{4.f, -2.f}};
	if (i1 >> o1)
		;
	// Extention correctness
	{
	circuit::Board 
		b {std::vector<std::variant<InWire, OutWire>>{std::variant<InWire, OutWire>{i1}}};
	{
	size_t vec_sz = b.vec().size();
	b += o1;
	ASSERT_THAT(b.vec().size(), ::testing::Eq(vec_sz + 1));
	}

	EXPECT_THAT(std::get<InWire>(b[0uz]).getuv(), ::testing::Eq(i1.getuv()));
	EXPECT_THAT(std::get<OutWire>(b[1uz]).getuv(), ::testing::Eq(o1.getuv()));

	ASSERT_THAT(std::get<InWire>(b[0uz]).is_tethered(), ::testing::IsTrue());
	ASSERT_THAT(std::get<InWire>(b[0uz]).tethered_view().value().get(), ::testing::Ref(std::get<OutWire>(b[1uz])));
	ASSERT_THAT(std::get<OutWire>(b[1uz]).is_tethered(), ::testing::IsTrue());
	ASSERT_THAT(std::get<OutWire>(b[1uz]).tethered_view().value().get(), ::testing::Ref(std::get<InWire>(b[0uz])));
	}
}

TEST(CreateFromMany, Board) {
	InWire i1 {std::pair{1.f, -4.f}};
	OutWire o1 {std::pair{4.f, -2.f}};
	if (i1 >> o1)
		;
	// Copy/move/assign correctness
	{
	// Create from many check
	circuit::Board b {std::vector<std::variant<InWire, OutWire>>{i1, o1}};
	ASSERT_THAT(b.vec().size(), ::testing::Eq(2uz));

	EXPECT_THAT(std::get<InWire>(b[0uz]).getuv(), ::testing::Eq(i1.getuv()));
	EXPECT_THAT(std::get<OutWire>(b[1uz]).getuv(), ::testing::Eq(o1.getuv()));

	ASSERT_THAT(std::get<InWire>(b[0uz]).is_tethered(), ::testing::IsTrue());
	EXPECT_THAT(std::get<InWire>(b[0uz]).tethered_view().value().get(), ::testing::Ref(std::get<OutWire>(b[1uz])));
	ASSERT_THAT(std::get<OutWire>(b[1uz]).is_tethered(), ::testing::IsTrue());
	EXPECT_THAT(std::get<OutWire>(b[1uz]).tethered_view().value().get(), ::testing::Ref(std::get<InWire>(b[0uz])));
	}
}

TEST(CopyMoveAssign, Board) {
	InWire i1 {std::pair{1.f, -4.f}};
	OutWire o1 {std::pair{4.f, -2.f}};
	if (i1 >> o1)
		;
	// Copy/move/assign correctness
	{
	// Create from many check
	circuit::Board b {std::vector<std::variant<InWire, OutWire>>{i1, o1}};

	circuit::Board bc {b};
	circuit::Board bca = b;

	circuit::Board b1 {b};
	circuit::Board b2 {b};
	circuit::Board bm {std::move(b1)};
	circuit::Board bma = std::move(b2);

	for (auto& it : std::vector{bc, bca, bm, bma}) {
		ASSERT_THAT(it.vec().size(), ::testing::Eq(b.vec().size()));
		ASSERT_THAT(std::get<InWire>(it[0uz]).is_tethered(), ::testing::IsTrue());
		ASSERT_THAT(std::get<InWire>(b[0uz]).tethered_view().value().get(), ::testing::Ref(std::get<OutWire>(b[1uz])));
		ASSERT_THAT(std::get<OutWire>(b[1uz]).is_tethered(), ::testing::IsTrue());
		ASSERT_THAT(std::get<OutWire>(b[1uz]).tethered_view().value().get(), ::testing::Ref(std::get<InWire>(b[0uz])));
	}
	}
}

// NOTE: As Wire classes are presumed to be fully verified by this point, they are fully polymorphic,
// hence there's no point to reversing i1 and o1 in the tests above like it would, if their behaviour wasn't guranteed to be fully symmetrical with respect to direction

TEST(AddLink, Board) {
	InWire i1 {std::pair{1.f, -2.f}};
	OutWire o1 {std::pair{3.f, -4.f}};
	InWire i2 {std::pair{5.f, -6.f}};
	OutWire o2{std::pair{7.f, -8.f}};
	if(!(i2 >> o2))
		FAIL();
	circuit::Board b {std::vector<std::variant<InWire, OutWire>>{i1, o1, i2, o2}};

	if(!b.add_link(0uz,1uz))
		FAIL();

	// Preserves vector in the valid state
	ASSERT_EQ(b.vec().size(), 4uz);
	EXPECT_EQ(std::get<InWire>(b[0uz]).getuv(), i1.getuv());
	EXPECT_EQ(std::get<OutWire>(b[1uz]).getuv(), o1.getuv());
	EXPECT_EQ(std::get<InWire>(b[2uz]).getuv(), i2.getuv());
	EXPECT_EQ(std::get<OutWire>(b[3uz]).getuv(), o2.getuv());
	ASSERT_TRUE(std::get<InWire>(b[2uz]).is_tethered());
	ASSERT_TRUE(std::get<OutWire>(b[3uz]).is_tethered());
	EXPECT_THAT(std::get<InWire>(b[2uz]), ::testing::Ref(std::get<OutWire>(b[3uz])));
	EXPECT_THAT(std::get<OutWire>(b[3uz]), ::testing::Ref(std::get<InWire>(b[2uz])));

	// Binds two wires together in the orderly manner
	ASSERT_TRUE(std::get<InWire>(b[0uz]).is_tethered());
	EXPECT_THAT(std::get<InWire>(b[0uz]).tethered_view().value().get(), ::testing::Ref(std::get<OutWire>(b[1uz])));
	ASSERT_TRUE(std::get<InWire>(b[1uz]).is_tethered());
	EXPECT_THAT(std::get<InWire>(b[1uz]).tethered_view().value().get(), ::testing::Ref(std::get<OutWire>(b[0uz])));
}

TEST(RmLink, Board) {
	InWire i1 {std::pair{1.f, -2.f}};
	OutWire o1 {std::pair{3.f, -4.f}};
	InWire i2 {std::pair{5.f, -6.f}};
	OutWire o2{std::pair{7.f, -8.f}};
	if(!(i2 >> o2))
		FAIL();
	if(!(i1 >> o1))
		FAIL();

	circuit::Board b {std::vector<std::variant<InWire, OutWire>>{i1, o1, i2, o2}};

	if(!b.add_link(0uz,1uz))
		FAIL();

	// Preserves vector in the valid state
	ASSERT_EQ(b.vec().size(), 4uz);
	EXPECT_EQ(std::get<InWire>(b[0uz]).getuv(), i1.getuv());
	EXPECT_EQ(std::get<OutWire>(b[1uz]).getuv(), o1.getuv());
	EXPECT_EQ(std::get<InWire>(b[2uz]).getuv(), i2.getuv());
	EXPECT_EQ(std::get<OutWire>(b[3uz]).getuv(), o2.getuv());
	ASSERT_TRUE(std::get<InWire>(b[2uz]).is_tethered());
	ASSERT_TRUE(std::get<OutWire>(b[3uz]).is_tethered());
	EXPECT_THAT(std::get<InWire>(b[2uz]).tethered_view().value().get(), ::testing::Ref(std::get<OutWire>(b[3uz])));  // !!!!!
	EXPECT_THAT(std::get<OutWire>(b[3uz]).tethered_view().value().get(), ::testing::Ref(std::get<InWire>(b[2uz])));  // !!!!!

	// Removes the binding between two wires in the orderly manner
	ASSERT_FALSE(std::get<InWire>(b[0uz]).is_tethered());
	ASSERT_FALSE(std::get<InWire>(b[1uz]).is_tethered());
}

TEST(RemoveEl, Board) {
	InWire i1 {std::pair{1.f, -2.f}};
	OutWire o1 {std::pair{3.f, -4.f}};
	InWire i2 {std::pair{5.f, -6.f}};
	OutWire o2{std::pair{7.f, -8.f}};
	if(!(i2 >> o2))
		FAIL();
	if(!(i1 >> o1))
		FAIL();

	circuit::Board b {std::vector<std::variant<InWire, OutWire>>{i1, o1, i2, o2}};

	if(!b.remove(1uz))
		FAIL();

	// Preserves vector in the valid state
	ASSERT_EQ(b.vec().size(), 3uz);
	EXPECT_EQ(std::get<OutWire>(b[0uz]).getuv(), o1.getuv());
	EXPECT_EQ(std::get<InWire>(b[1uz]).getuv(), i2.getuv());
	EXPECT_EQ(std::get<OutWire>(b[2uz]).getuv(), o2.getuv());
	ASSERT_TRUE(std::get<InWire>(b[1uz]).is_tethered());
	ASSERT_TRUE(std::get<OutWire>(b[2uz]).is_tethered());
	EXPECT_THAT(std::get<InWire>(b[1uz]).tethered_view().value().get(), ::testing::Ref(std::get<OutWire>(b[3uz]))); // !!!!!!
	EXPECT_THAT(std::get<OutWire>(b[2uz]).tethered_view().value().get(), ::testing::Ref(std::get<InWire>(b[2uz]))); // !!!!

	// Binds two wires together in the orderly manner
	ASSERT_FALSE(std::get<InWire>(b[0uz]).is_tethered());
	ASSERT_TRUE(std::get<InWire>(b[2uz]).is_tethered());
	EXPECT_THAT(std::get<InWire>(b[2uz]).tethered_view().value().get(), ::testing::Ref(std::get<OutWire>(b[1uz])));
	ASSERT_TRUE(std::get<InWire>(b[1uz]).is_tethered());
	EXPECT_THAT(std::get<InWire>(b[1uz]).tethered_view().value().get(), ::testing::Ref(std::get<OutWire>(b[2uz])));
}

TEST(MoveUV, Board) {
	InWire i1 {std::pair{1.f, -2.f}};
	OutWire o1 {std::pair{3.f, -4.f}};
	InWire i2 {std::pair{5.f, -6.f}};
	OutWire o2{std::pair{7.f, -8.f}};
	if(!(i2 >> o2))
		FAIL();
	circuit::Board b {std::vector<std::variant<InWire, OutWire>>{i1, o1, i2, o2}};

	if(!b.moveuv(0uz,{9.f, -10.f}))
		FAIL();
	if(!b.moveuv(3uz,{11.f, -12.f}))
		FAIL();

	// Preserves vector in the valid state && changes the uv coordinates of only the elements specified.
	ASSERT_EQ(b.vec().size(), 4uz);
	EXPECT_THAT(std::get<InWire>(b[0uz]).getuv(), ::testing::Eq(std::pair<float, float>(9.f, -10.f)));
	EXPECT_EQ(std::get<OutWire>(b[1uz]).getuv(), o1.getuv());
	EXPECT_EQ(std::get<InWire>(b[2uz]).getuv(), i2.getuv());
	EXPECT_THAT(std::get<OutWire>(b[3uz]).getuv(), ::testing::Eq(std::pair{11.f, -12.f}));
	ASSERT_TRUE(std::get<InWire>(b[2uz]).is_tethered());
	ASSERT_TRUE(std::get<OutWire>(b[3uz]).is_tethered());
	EXPECT_FALSE(std::get<InWire>(b[0uz]).is_tethered()); 
	EXPECT_FALSE(std::get<OutWire>(b[1uz]).is_tethered());
	EXPECT_THAT(std::get<InWire>(b[2uz]), ::testing::Ref(std::get<OutWire>(b[3uz])));
	EXPECT_THAT(std::get<OutWire>(b[3uz]), ::testing::Ref(std::get<InWire>(b[2uz])));
}
