/** @file testPartitioning.cxx
 ** Unit Tests for Scintilla internal data structures
 **/

#include <cstddef>
#include <cstring>

#include <stdexcept>
#include <string_view>
#include <vector>
#include <optional>
#include <algorithm>
#include <memory>

#include "Debugging.h"

#include "Position.h"
#include "SplitVector.h"
#include "Partitioning.h"

#include "catch.hpp"

using namespace Scintilla::Internal;

// Test Partitioning.

TEST_CASE("CompileCopying Partitioning") {

	// These are compile-time tests to check that basic copy and move
	// operations are defined correctly.

	SECTION("CopyingMoving") {
		Partitioning<int> s;
		Partitioning<int> s2;

		// Copy constructor
		Partitioning<int> sa(s);
		// Copy assignment
		Partitioning<int> sb;
		sb = s;

		// Move constructor
		Partitioning<int> sc(std::move(s));
		// Move assignment
		Partitioning<int> sd;
		sd = (std::move(s2));
	}

}

TEST_CASE("Partitioning") {

	Partitioning<Sci::Position> part;

	SECTION("IsEmptyInitially") {
		REQUIRE(1 == part.Partitions());
		REQUIRE(0 == part.PositionFromPartition(part.Partitions()));
		REQUIRE(0 == part.PartitionFromPosition(0));
	}

	SECTION("SimpleInsert") {
		part.InsertText(0, 1);
		REQUIRE(1 == part.Partitions());
		REQUIRE(1 == part.PositionFromPartition(part.Partitions()));
	}

	SECTION("TwoPartitions") {
		part.InsertText(0, 2);
		part.InsertPartition(1, 1);
		REQUIRE(2 == part.Partitions());
		REQUIRE(0 == part.PositionFromPartition(0));
		REQUIRE(1 == part.PositionFromPartition(1));
		REQUIRE(2 == part.PositionFromPartition(2));
	}

	SECTION("MoveStart") {
		part.InsertText(0, 3);
		part.InsertPartition(1, 2);
		part.SetPartitionStartPosition(1,1);
		REQUIRE(2 == part.Partitions());
		REQUIRE(0 == part.PositionFromPartition(0));
		REQUIRE(1 == part.PositionFromPartition(1));
		REQUIRE(3 == part.PositionFromPartition(2));
		part.Check();
	}

	SECTION("InsertAgain") {
		part.InsertText(0, 3);
		part.InsertPartition(1, 2);
		part.InsertText(0,3);
		part.InsertText(1,2);
		REQUIRE(2 == part.Partitions());
		REQUIRE(0 == part.PositionFromPartition(0));
		REQUIRE(5 == part.PositionFromPartition(1));
		REQUIRE(8 == part.PositionFromPartition(2));
		part.Check();
	}

	SECTION("InsertMultiple") {
		part.InsertText(0, 10);
		const Sci::Position positions[] { 2, 5, 7 };
		part.InsertPartitions(1, positions, std::size(positions));
		REQUIRE(4 == part.Partitions());
		REQUIRE(0 == part.PositionFromPartition(0));
		REQUIRE(2 == part.PositionFromPartition(1));
		REQUIRE(5 == part.PositionFromPartition(2));
		REQUIRE(7 == part.PositionFromPartition(3));
		REQUIRE(10 == part.PositionFromPartition(4));
		part.Check();
	}

	SECTION("InsertMultipleWithCast") {
		part.InsertText(0, 9);
		REQUIRE(1 == part.Partitions());
		const ptrdiff_t positionsp[]{ 2, 4, 6, 8 };
		part.InsertPartitionsWithCast(1, positionsp, std::size(positionsp));
		REQUIRE(5 == part.Partitions());
		REQUIRE(0 == part.PositionFromPartition(0));
		REQUIRE(2 == part.PositionFromPartition(1));
		REQUIRE(4 == part.PositionFromPartition(2));
		REQUIRE(6 == part.PositionFromPartition(3));
		REQUIRE(8 == part.PositionFromPartition(4));
		REQUIRE(9 == part.PositionFromPartition(5));
		part.Check();
	}

	SECTION("InsertReversed") {
		part.InsertText(0, 3);
		part.InsertPartition(1, 2);
		part.InsertText(1,2);
		part.InsertText(0,3);
		REQUIRE(2 == part.Partitions());
		REQUIRE(0 == part.PositionFromPartition(0));
		REQUIRE(5 == part.PositionFromPartition(1));
		REQUIRE(8 == part.PositionFromPartition(2));
		part.Check();
	}

	SECTION("InverseSearch") {
		part.InsertText(0, 3);
		part.InsertPartition(1, 2);
		part.SetPartitionStartPosition(1,1);

		REQUIRE(2 == part.Partitions());
		REQUIRE(0 == part.PositionFromPartition(0));
		REQUIRE(1 == part.PositionFromPartition(1));
		REQUIRE(3 == part.PositionFromPartition(2));

		REQUIRE(0 == part.PartitionFromPosition(0));
		REQUIRE(1 == part.PartitionFromPosition(1));
		REQUIRE(1 == part.PartitionFromPosition(2));

		REQUIRE(1 == part.PartitionFromPosition(3));
		part.Check();
	}

	SECTION("DeletePartition") {
		part.InsertText(0, 2);
		part.InsertPartition(1, 1);
		part.RemovePartition(1);
		REQUIRE(1 == part.Partitions());
		REQUIRE(0 == part.PositionFromPartition(0));
		REQUIRE(2 == part.PositionFromPartition(1));
		part.Check();
	}

	SECTION("DeleteAll") {
		part.InsertText(0, 3);
		part.InsertPartition(1, 2);
		part.SetPartitionStartPosition(1,1);
		part.DeleteAll();
		// Back to initial state
		REQUIRE(1 == part.Partitions());
		REQUIRE(0 == part.PositionFromPartition(part.Partitions()));
	}

	SECTION("TestBackwards") {
		part.InsertText(0, 10);
		part.InsertPartition(1, 3);
		part.InsertPartition(2, 6);
		part.InsertPartition(3, 9);
		part.InsertText(2,4);
		part.InsertText(1,2);
		part.InsertText(0,3);
		REQUIRE(4 == part.Partitions());
		REQUIRE(0 == part.PositionFromPartition(0));
		REQUIRE(6 == part.PositionFromPartition(1));
		REQUIRE(11 == part.PositionFromPartition(2));
		REQUIRE(18 == part.PositionFromPartition(3));
		REQUIRE(19 == part.PositionFromPartition(4));
		part.Check();
	}

	SECTION("TestMany") {
		// Provoke backstep call
		part.InsertText(0, 42);
		for (int i=0; i<20; i++) {
			part.InsertPartition(i+1, (i+1) * 2);
		}
		for (int i=20; i>0; i--) {
			part.InsertText(i,2);
		}
		REQUIRE(21 == part.Partitions());
		for (int i=1; i<20; i++) {
			REQUIRE((i*4 - 2) == part.PositionFromPartition(i));
			REQUIRE(i == part.PartitionFromPosition(i*4 - 2));
		}
		part.InsertText(19,2);
		REQUIRE(3 == part.PartitionFromPosition(10));
		part.InsertText(0,2);
		part.InsertText(0,-2);
		part.RemovePartition(1);
		REQUIRE(0 == part.PositionFromPartition(0));
		REQUIRE(6 == part.PositionFromPartition(1));
		REQUIRE(10 == part.PositionFromPartition(2));
		part.RemovePartition(10);
		REQUIRE(46 == part.PositionFromPartition(10));
		REQUIRE(10 == part.PartitionFromPosition(46));
		REQUIRE(50 == part.PositionFromPartition(11));
		REQUIRE(11 == part.PartitionFromPosition(50));
		part.Check();
	}

}
