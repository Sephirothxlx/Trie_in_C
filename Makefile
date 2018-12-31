test:trie.c trie.h trie_test.c
	gcc -std=c11 -Wall -Werror -pedantic -o trie_test trie_test.c trie.c -lcunit
	./trie_test

clean:
	rm -f *.o trie_test
