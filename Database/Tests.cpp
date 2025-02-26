#include <gtest/gtest.h>
#include "Database.h"

TEST(DatabaseTest, BasicSetAndGet) {
    Database db;
    db.set("foo", "bar");
    EXPECT_EQ(db.get("foo"), "bar");
}

TEST(DatabaseTest, DelKey) {
    Database db;
    db.set("foo", "bar");
    bool deleted = db.del("foo");
    EXPECT_TRUE(deleted);
    EXPECT_EQ(db.get("foo"), "");
}
