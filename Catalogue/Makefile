FLAG = -Wall -Wextra -std=c++17 -ltbb -pthread -g

all: tc

tc: transport_catalogue.o main.o json.o json_builder.o json_reader.o domain.o svg.o geo.o map_renderer.o transport_router.o
	g++ -o tc main.o transport_catalogue.o json.o json_builder.o json_reader.o domain.o svg.o geo.o map_renderer.o transport_router.o
	rm -rf *.o *.gch
	./tc < tests/new_tests/test_3.json

test: clean tc 

main.o:	main.cpp
	g++ -c $(FLAG) main.cpp

transport_catalogue.o: transport_catalogue.cpp
	g++ -c $(FLAG) transport_catalogue.cpp transport_catalogue.h

json.o: json.cpp
	g++ -c $(FLAG) json.cpp json.h

json_reader.o: json_reader.cpp
	g++ -c $(FLAG) json_reader.cpp json_reader.h

domain.o: domain.cpp
	g++ -c $(FLAG) domain.cpp domain.h

svg.o: svg.cpp
	g++ -c $(FLAG) svg.cpp svg.h

geo.o: geo.cpp
	g++ -c $(FLAG) geo.cpp geo.h

map_renderer.o: map_renderer.cpp
	g++ -c $(FLAG) map_renderer.cpp map_renderer.h

json_builder.o: json_builder.cpp
	g++ -c $(FLAG) json_builder.cpp json_builder.h

transport_router.o: transport_router.cpp
	g++ -c $(FLAG) transport_router.cpp transport_router.h

cppcheck:
	cppcheck --enable=all --suppress=missingIncludeSystem .

run:
	./tc < tests/new_tests/test_1.json > tests/result/ans_1.json
	./tc < tests/new_tests/test_2.json > tests/result/ans_2.json
	./tc < tests/new_tests/test_3.json > tests/result/ans_3.json
	./tc < tests/new_tests/test_4.json > tests/result/ans_4.json
	./tc < tests/new_tests/test_5.json > tests/result/ans_5.json

clean:	
	rm -rf *.o *.gch *.exe *.svg tc