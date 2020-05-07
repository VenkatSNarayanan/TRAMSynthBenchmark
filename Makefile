tram:
	charmc test.ci -O3 && charmc test.C -O3 -o tram

tram2:
	charmc test2.ci -O3 && charmc test.C -O3 -o tram2

notram:
	charmc notest.ci -O3 && charmc test.C -O3 -o notram

clean:
	rm *.h charmrun tram tram2 notram

.PHONY: tram tram2 notram clean
