#include "MyCoolDB.h"

#include "gtest/gtest.h"

TEST(Data_Base_TestSuite, CreateTableTest) {
    MyCoolDB data_base;

    data_base.Request("CREATE TABLE Students ("
               "    isu    INT,"
               "    name  VARCHAR,"
               "    points   DOUBLE"
               ");");
}

TEST(Data_Base_TestSuite, InsertTest) {
    MyCoolDB data_base;

    data_base.Request("CREATE TABLE Students ("
               "    isu    INT,"
               "    name  VARCHAR,"
               "    points   DOUBLE,"
               "    budget  BOOL"
               ");");

    data_base.Request("INSERT INTO Students(isu, name, points, budget) VALUES (1, 'Ivan Petrov', 74.1, true)");
    data_base.Request("INSERT INTO Students(isu, name) VALUES (2, 'Petr Ivanov')");
    data_base.Request("INSERT INTO Students(isu, name, points) VALUES (3, 'Sergey Sidorov', 59.9)");
    data_base.Request("INSERT INTO Students(isu, name, budget) VALUES (4, 'Pavel Drozdov', false)");

    Result result = data_base.RequestQuery("SELECT * FROM Students");
    ASSERT_EQ(result.Count(), 4);

    result.Next();
    ASSERT_EQ(result.Get("isu").GetIntValue(), 1);
    ASSERT_EQ(result.Get("name").GetStringValue(), "Ivan Petrov");
    ASSERT_EQ(result.Get("points").GetDoubleValue(), 74.1);
    ASSERT_EQ(result.Get("budget").GetBoolValue(), true);

    result.Next();
    ASSERT_EQ(result.Get("isu").GetIntValue(), 2);
    ASSERT_EQ(result.Get("name").GetStringValue(), "Petr Ivanov");
    ASSERT_TRUE(result.Get("points").IsNull());
    ASSERT_TRUE(result.Get("budget").IsNull());

    result.Next();
    ASSERT_EQ(result.Get("isu").GetIntValue(), 3);
    ASSERT_EQ(result.Get("name").GetStringValue(), "Sergey Sidorov");
    ASSERT_EQ(result.Get("points").GetDoubleValue(), 59.9);
    ASSERT_TRUE(result.Get("budget").IsNull());

    result.Next();
    ASSERT_EQ(result.Get("isu").GetIntValue(), 4);
    ASSERT_EQ(result.Get("name").GetStringValue(), "Pavel Drozdov");
    ASSERT_TRUE(result.Get("points").IsNull());
    ASSERT_EQ(result.Get("budget").GetBoolValue(), false);

    ASSERT_FALSE(result.Next());
}

TEST(Data_Base_TestSuite, SelectTest) {
    MyCoolDB data_base;

    data_base.Request("CREATE TABLE Students ("
               "    isu    INT,"
               "    name  VARCHAR,"
               "    points   DOUBLE,"
               "    budget  BOOL"
               ");");

    data_base.Request("INSERT INTO Students(isu, name, points, budget) VALUES (1, 'Ivan Petrov',74.1, true)");
    data_base.Request("INSERT INTO Students(isu, name) VALUES (2, 'Petr Ivanov')");
    data_base.Request("INSERT INTO Students(isu, name, points) VALUES (3, 'Sergey Sidorov', 59.9)");
    data_base.Request("INSERT INTO Students(isu, name, budget) VALUES (4, 'Pavel Drozdov', false)");

    Result result1 = data_base.RequestQuery("SELECT isu, budget FROM Students WHERE name = 'Pavel Drozdov'");
    ASSERT_EQ(result1.Count(), 1);
    ASSERT_EQ(result1.Get("isu").GetIntValue(), 4);
    ASSERT_EQ(result1.Get("budget").GetBoolValue(), false);

    Result result2 = data_base.RequestQuery("SELECT name FROM Students WHERE isu <= 2");
    ASSERT_EQ(result2.Count(), 2);
    result2.Next();
    ASSERT_EQ(result2.Get("name").GetStringValue(), "Ivan Petrov");
    result2.Next();
    ASSERT_EQ(result2.Get("name").GetStringValue(), "Petr Ivanov");
    ASSERT_FALSE(result2.Next());

    Result result3 = data_base.RequestQuery("SELECT name, points FROM Students WHERE points <= 60 AND budget IS NULL");
    ASSERT_EQ(result3.Count(), 1);
    ASSERT_EQ(result3.Get("name").GetStringValue(), "Sergey Sidorov");
    ASSERT_EQ(result3.Get("points").GetDoubleValue(), 59.9);
    ASSERT_FALSE(result2.Next());

    Result result4 = data_base.RequestQuery("SELECT isu FROM Students WHERE isu > 3 OR points IS NOT NULL");
    ASSERT_EQ(result4.Count(), 3);
    result4.Next();
    ASSERT_EQ(result4.Get("isu").GetIntValue(), 1);
    result4.Next();
    ASSERT_EQ(result4.Get("isu").GetIntValue(), 3);
    result4.Next();
    ASSERT_EQ(result4.Get("isu").GetIntValue(), 4);
    ASSERT_FALSE(result2.Next());

    Result result5 = data_base.RequestQuery("SELECT isu FROM Students WHERE budget != true");
    ASSERT_EQ(result5.Count(), 1);
    ASSERT_EQ(result5.Get("isu").GetIntValue(), 4);
    ASSERT_FALSE(result5.Next());
}

TEST(Data_Base_TestSuite, UpdateTest) {
    MyCoolDB data_base;

    data_base.Request("CREATE TABLE Students ("
               "    isu    INT,"
               "    name  VARCHAR,"
               "    points   DOUBLE,"
               "    budget  BOOL"
               ");");

    data_base.Request("INSERT INTO Students(isu, name, points, budget) VALUES (1, 'Ivan Petrov',74.1, true)");
    data_base.Request("INSERT INTO Students(isu, name) VALUES (2, 'Petr Ivanov')");
    data_base.Request("INSERT INTO Students(isu, name, points) VALUES (3, 'Sergey Sidorov', 59.9)");
    data_base.Request("INSERT INTO Students(isu, name, budget) VALUES (4, 'Pavel Drozdov', false)");

    data_base.Request("UPDATE Students SET points = 60.1, budget = true WHERE isu = 2");
    data_base.Request("UPDATE Students SET points = NULL WHERE isu = 3");
    Result result = data_base.RequestQuery("SELECT * FROM Students WHERE isu = 2 OR isu = 3");
    ASSERT_EQ(result.Count(), 2);

    result.Next();
    ASSERT_EQ(result.Get("isu").GetIntValue(), 2);
    ASSERT_EQ(result.Get("points").GetDoubleValue(), 60.1);
    ASSERT_EQ(result.Get("budget").GetBoolValue(), true);

    result.Next();
    ASSERT_EQ(result.Get("isu").GetIntValue(), 3);
    ASSERT_TRUE(result.Get("points").IsNull());
}

TEST(Data_Base_TestSuite, DeleteTest) {
    MyCoolDB data_base;

    data_base.Request("CREATE TABLE Students ("
               "    isu    INT,"
               "    name  VARCHAR,"
               "    points   DOUBLE,"
               "    budget  BOOL"
               ");");

    data_base.Request("INSERT INTO Students(isu, name, points, budget) VALUES (1, 'Ivan Petrov',74.1, true)");
    data_base.Request("INSERT INTO Students(isu, name) VALUES (2, 'Petr Ivanov')");
    data_base.Request("INSERT INTO Students(isu, name, points) VALUES (3, 'Sergey Sidorov', 59.9)");
    data_base.Request("INSERT INTO Students(isu, name, budget) VALUES (4, 'Pavel Drozdov', false)");

    data_base.Request("DELETE FROM Students WHERE isu <= 2");
    Result result1 = data_base.RequestQuery("SELECT * FROM Students");
    ASSERT_EQ(result1.Count(), 2);

    data_base.Request("DELETE FROM Students");
    Result result2 = data_base.RequestQuery("SELECT * FROM Students");
    ASSERT_EQ(result2.Count(), 0);
}

TEST(Data_Base_TestSuite, DropTableTest) {
    MyCoolDB data_base;

    data_base.Request("CREATE TABLE Students ("
               "    isu    INT,"
               "    name  VARCHAR,"
               "    points   DOUBLE,"
               "    budget  BOOL"
               ");");

    data_base.Request("INSERT INTO Students(isu, name, points, budget) VALUES (1, 'Ivan Petrov',74.1, true)");
    data_base.Request("INSERT INTO Students(isu, name) VALUES (2, 'Petr Ivanov')");
    data_base.Request("INSERT INTO Students(isu, name, points) VALUES (3, 'Sergey Sidorov', 59.9)");
    data_base.Request("INSERT INTO Students(isu, name, budget) VALUES (4, 'Pavel Drozdov', false)");

    data_base.Request("CREATE TABLE Test ("
               "    flag BOOL"
               ")");

    ASSERT_EQ(data_base.GetTableNames(), std::vector<std::string>({"Students", "Test"}));
    data_base.Request("DROP TABLE Students");
    ASSERT_EQ(data_base.GetTableNames(), std::vector<std::string>({"Test"}));
    data_base.Request("DROP TABLE Test");
    ASSERT_EQ(data_base.GetTableNames(), std::vector<std::string>());
}

TEST(Data_Base_TestSuite, PrimaryKeyTest) {
    MyCoolDB data_base;
    data_base.Request("CREATE TABLE Subject ("
               "    priority INT PRIMARY KEY,"
               "    name VARCHAR"
               ")");

    data_base.Request("INSERT INTO Subject(priority, name) VALUES (1, 'C++')");
    ASSERT_THROW(data_base.Request("INSERT INTO Subject(priority, name) VALUES (1, 'C++')"), MyBDException);
    data_base.Request("INSERT INTO Subject(priority, name) VALUES (2, 'Math')");
    ASSERT_THROW(data_base.Request("INSERT INTO Subject(priority, name) VALUES (2, 'Math')"), MyBDException);
}

TEST(Data_Base_TestSuite, ForeignKeyTest) {
    MyCoolDB data_base;

    data_base.Request("CREATE TABLE Students ("
               "    isu    INT,"
               "    name  VARCHAR,"
               "    points   DOUBLE,"
               "    budget  BOOL"
               ");");

    data_base.Request("INSERT INTO Students(isu, name, points, budget) VALUES (1, 'Ivan Petrov',74.1, true)");
    data_base.Request("INSERT INTO Students(isu, name) VALUES (2, 'Petr Ivanov')");
    data_base.Request("INSERT INTO Students(isu, name, points) VALUES (3, 'Sergey Sidorov', 59.9)");
    data_base.Request("INSERT INTO Students(isu, name, budget) VALUES (4, 'Pavel Drozdov', false)");

    data_base.Request("CREATE TABLE Food ("
               "    id INT PRIMARY KEY,"
               "    customer_id INT,"
               "    product VARCHAR,"
               "    FOREIGN KEY (customer_id) REFERENCES Students(isu)"
               ")");

    data_base.Request("INSERT INTO Food(id, customer_id, product) VALUES (1, 1, 'Tea')");
    data_base.Request("INSERT INTO Food(id, customer_id, product) VALUES (2, 1, 'Kotleta')");
    data_base.Request("INSERT INTO Food(id, customer_id, product) VALUES (3, 2, 'Coffee')");
    data_base.Request("INSERT INTO Food(id, customer_id, product) VALUES (4, 2, 'RedBull')");
    data_base.Request("INSERT INTO Food(id, customer_id, product) VALUES (5, 3, 'Adrenalin Rush')");
    data_base.Request("INSERT INTO Food(id, customer_id, product) VALUES (6, 4, 'Potatoes')");
    ASSERT_THROW(data_base.Request("INSERT INTO Food(id, customer_id, product) VALUES (7, 5, 'Cheese')"), MyBDException);
    ASSERT_THROW(data_base.Request("INSERT INTO Food(id, customer_id, product) VALUES (8, 6, 'Bread')"), MyBDException);
}

TEST(Data_Base_TestSuite, SaveLoadTest) {
    MyCoolDB test_data_base;

    test_data_base.Request("CREATE TABLE Students ("
               "    isu    INT,"
               "    name  VARCHAR,"
               "    points   DOUBLE,"
               "    budget  BOOL"
               ");");

    test_data_base.Request("INSERT INTO Students(isu, name, points, budget) VALUES (1, 'Ivan Petrov',74.1, true)");
    test_data_base.Request("INSERT INTO Students(isu, name) VALUES (2, 'Petr Ivanov')");
    test_data_base.Request("INSERT INTO Students(isu, name, points) VALUES (3, 'Sergey Sidorov', 59.9)");
    test_data_base.Request("INSERT INTO Students(isu, name, budget) VALUES (4, 'Pavel Drozdov', false)");
    test_data_base.Save("test.data_base");

    MyCoolDB data_base;
    data_base.Load("test.data_base");

    Result result = data_base.RequestQuery("SELECT * FROM Students");
    ASSERT_EQ(result.Count(), 4);

    result.Next();
    ASSERT_EQ(result.Get("isu").GetIntValue(), 1);
    ASSERT_EQ(result.Get("name").GetStringValue(), "Ivan Petrov");
    ASSERT_EQ(result.Get("points").GetDoubleValue(), 74.1);
    ASSERT_EQ(result.Get("budget").GetBoolValue(), true);

    result.Next();
    ASSERT_EQ(result.Get("isu").GetIntValue(), 2);
    ASSERT_EQ(result.Get("name").GetStringValue(), "Petr Ivanov");
    ASSERT_TRUE(result.Get("points").IsNull());
    ASSERT_TRUE(result.Get("budget").IsNull());

    result.Next();
    ASSERT_EQ(result.Get("isu").GetIntValue(), 3);
    ASSERT_EQ(result.Get("name").GetStringValue(), "Sergey Sidorov");
    ASSERT_EQ(result.Get("points").GetDoubleValue(), 59.9);
    ASSERT_TRUE(result.Get("budget").IsNull());

    result.Next();
    ASSERT_EQ(result.Get("isu").GetIntValue(), 4);
    ASSERT_EQ(result.Get("name").GetStringValue(), "Pavel Drozdov");
    ASSERT_TRUE(result.Get("points").IsNull());
    ASSERT_EQ(result.Get("budget").GetBoolValue(), false);

    ASSERT_FALSE(result.Next());
}

TEST(Data_Base_TestSuite, JoinTest) {
    MyCoolDB data_base;

    data_base.Request("CREATE TABLE Students ("
               "    isu    INT,"
               "    name  VARCHAR,"
               "    points   DOUBLE,"
               "    budget  BOOL"
               ");");

    data_base.Request("INSERT INTO Students(isu, name, points, budget) VALUES (1, 'Ivan Petrov',74.1, true)");
    data_base.Request("INSERT INTO Students(isu, name) VALUES (2, 'Petr Ivanov')");
    data_base.Request("INSERT INTO Students(isu, name, points) VALUES (3, 'Sergey Sidorov', 59.9)");
    data_base.Request("INSERT INTO Students(isu, name, budget) VALUES (4, 'Pavel Drozdov', false)");

    data_base.Request("CREATE TABLE Food ("
               "    id INT PRIMARY KEY,"
               "    customer_id INT,"
               "    product VARCHAR,"
               ")");

    data_base.Request("INSERT INTO Food(id, customer_id, product) VALUES (1, 1, 'Tea')");
    data_base.Request("INSERT INTO Food(id, customer_id, product) VALUES (2, 1, 'Kotleta')");
    data_base.Request("INSERT INTO Food(id, customer_id, product) VALUES (3, 2, 'Chocolate')");
    data_base.Request("INSERT INTO Food(id, customer_id, product) VALUES (4, 2, 'RedBull')");
    data_base.Request("INSERT INTO Food(id, customer_id, product) VALUES (5, 4, 'Coffee')");
    data_base.Request("INSERT INTO Food(id, customer_id, product) VALUES (6, 5, 'Cheese')");
    data_base.Request("INSERT INTO Food(id, customer_id, product) VALUES (7, 6, 'Bread')");

    Result result1 = data_base.RequestQuery("SELECT * FROM Students INNER JOIN Food ON Food.customer_id = Students.isu");
    ASSERT_EQ(result1.Count(), 5);
    result1.Next();
    ASSERT_EQ(result1.Get("Food.id").GetIntValue(), 1);
    ASSERT_EQ(result1.Get("Students.isu").GetIntValue(), 1);
    ASSERT_EQ(result1.Get("Students.name").GetStringValue(), "Ivan Petrov");
    ASSERT_EQ(result1.Get("Food.product").GetStringValue(), "Tea");

    result1.Next();
    ASSERT_EQ(result1.Get("Food.id").GetIntValue(), 2);
    ASSERT_EQ(result1.Get("Students.isu").GetIntValue(), 1);
    ASSERT_EQ(result1.Get("Students.name").GetStringValue(), "Ivan Petrov");
    ASSERT_EQ(result1.Get("Food.product").GetStringValue(), "Kotleta");

    result1.Next();
    ASSERT_EQ(result1.Get("Food.id").GetIntValue(), 3);
    ASSERT_EQ(result1.Get("Students.isu").GetIntValue(), 2);
    ASSERT_EQ(result1.Get("Students.name").GetStringValue(), "Petr Ivanov");
    ASSERT_EQ(result1.Get("Food.product").GetStringValue(), "Chocolate");

    result1.Next();
    ASSERT_EQ(result1.Get("Food.id").GetIntValue(), 4);
    ASSERT_EQ(result1.Get("Students.isu").GetIntValue(), 2);
    ASSERT_EQ(result1.Get("Students.name").GetStringValue(), "Petr Ivanov");
    ASSERT_EQ(result1.Get("Food.product").GetStringValue(), "RedBull");

    result1.Next();
    ASSERT_EQ(result1.Get("Food.id").GetIntValue(), 5);
    ASSERT_EQ(result1.Get("Students.isu").GetIntValue(), 4);
    ASSERT_EQ(result1.Get("Students.name").GetStringValue(), "Pavel Drozdov");
    ASSERT_EQ(result1.Get("Food.product").GetStringValue(), "Coffee");
    ASSERT_FALSE(result1.Next());


    Result result2 = data_base.RequestQuery("SELECT * FROM Students LEFT JOIN Food ON Food.customer_id = Students.isu");
    ASSERT_EQ(result2.Count(), 6);
    result2.Next();
    result2.Next();
    result2.Next();
    result2.Next();
    result2.Next();
    ASSERT_TRUE(result2.Get("Food.id").IsNull());
    ASSERT_TRUE(result2.Get("Food.customer_id").IsNull());
    ASSERT_TRUE(result2.Get("Food.product").IsNull());
    ASSERT_EQ(result2.Get("Students.isu").GetIntValue(), 3);
    ASSERT_EQ(result2.Get("Students.name").GetStringValue(), "Sergey Sidorov");

    Result result3 = data_base.RequestQuery("SELECT * FROM Students RIGHT JOIN Food ON Food.customer_id = Students.isu");
    ASSERT_EQ(result3.Count(), 7);
    result3.Next();
    result3.Next();
    result3.Next();
    result3.Next();
    result3.Next();
    result3.Next();
    ASSERT_TRUE(result3.Get("Students.isu").IsNull());
    ASSERT_TRUE(result3.Get("Students.name").IsNull());
    ASSERT_TRUE(result3.Get("Students.points").IsNull());
    ASSERT_EQ(result3.Get("Food.id").GetIntValue(), 6);
    ASSERT_EQ(result3.Get("Food.product").GetStringValue(), "Cheese");

    result3.Next();
    ASSERT_TRUE(result3.Get("Students.isu").IsNull());
    ASSERT_TRUE(result3.Get("Students.name").IsNull());
    ASSERT_TRUE(result3.Get("Students.points").IsNull());
    ASSERT_EQ(result3.Get("Food.id").GetIntValue(), 7);
ASSERT_EQ(result3.Get("Food.product").GetStringValue(), "Bread");


    Result result4 = data_base.RequestQuery("SELECT * FROM Students INNER JOIN Food ON Food.customer_id = Students.isu "
                                        "WHERE Students.isu >= 3 OR Food.product = 'Kotleta'");
    ASSERT_EQ(result4.Count(), 2);
    result4.Next();
    ASSERT_EQ(result4.Get("Food.id").GetIntValue(), 2);
    ASSERT_EQ(result4.Get("Students.isu").GetIntValue(), 1);
    ASSERT_EQ(result4.Get("Students.name").GetStringValue(), "Ivan Petrov");
    ASSERT_EQ(result4.Get("Food.product").GetStringValue(), "Kotleta");

    result4.Next();
    ASSERT_EQ(result4.Get("Food.id").GetIntValue(), 5);
    ASSERT_EQ(result4.Get("Students.isu").GetIntValue(), 4);
    ASSERT_EQ(result4.Get("Students.name").GetStringValue(), "Pavel Drozdov");
    ASSERT_EQ(result4.Get("Food.product").GetStringValue(), "Coffee");
}
