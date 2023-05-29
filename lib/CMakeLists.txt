include_directories(include)

add_library(MyCoolDB
        include/Table.h            src/Table.cpp
        include/MyCoolDB.h         src/MyCoolDB.cpp
        include/Result.h src/Result.cpp
        include/RequestParser.h src/RequestParser.cpp
        include/Column.h src/Column.cpp
        include/MyBDException.h
        include/Element.h           src/Element.cpp
)
