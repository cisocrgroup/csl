The compdic32u utility compiles a rewrite dictionary from a dictionary file in UNICODE format. 

The utility is invoked by:

compdic <alphabet> <text_dic> <info_dic> <comp_dic>

The alphabet is a file consisting of a line of characters. 

Each entry in the text_dic file has to be on a separate line in the form:

<key><TAB><value>

The file has to be sorted on the keys based on the order given by the alphabet.

The info_dic and comp_dic files are the resulting automata file (comp_dic) and the outputs file (info_dic).

IMPORTANT! The alphabet file and the dictionary text file have to be in 2-byte per character (UNICODE UCS-2 or UTF-16) format with low endian. (High endian should also work but I haven't tried it.)

===================================================================================

The rewrite32u and rewrite32nu utilities transduce a UNICODE text file with the help of a compiled rewite dictionary. The utility is invoked by:

rewrite32u <alphabet> <info_dic> <comp_dic>
rewrite32nu <alphabet> <info_dic> <comp_dic>

The alphabet has to be the same as above.

The info_dic and comp_dic files are the files of the compiled rewrite dictionary by the compdic utility.

The utility reads the standard input in UNICODE format.

The result is written out to the standard output in UNICODE format.


----

The difference between rewrite32u and rewrite32nu is the following: 

The first utility is appending to the occurences of dictionary words their corresponding value. If there are conflicts in the text or the dictionary (in respect of overlapping of dictionary words) they will not be resolved correctly and the resulting output could be wrong.

The second utility is replacing the dictionary entries with their corresponding value. All conflicts are solved using the "first-left-longest-match" strategy. The second utility is about twice slower as the first one.
