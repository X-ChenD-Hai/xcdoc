// re2c --lang c
/*!re2c
    //    ident_ =[a-zA-Z_][a-zA-Z0-9_]*;
    identchar = [^\x23\x2c\x2e\x3f\x21\x3a\x3b\x7e\x2d\x22\x27\x2f\x2a\x2b\x28\x29\x3c\x3e\x7b\x7d\x5b\x5d\x3d\x25\x26\x7c\x5c\x5e\x09\x0a\x0d\x00 0-9]; 
    ident = (identchar|[\\][\n]) (identchar|[\\][\n]|[0-9])*;
    whitespace = [ \t]*;
    _str1 = ["] ([\\]["]|[^\x00])* ["];
    _str2 = [R]["] ident? "(" [^\x00]* ")" ident? ["];
    str = _str1|_str2;
    // str = ["] (([\\]["])|[^\x00])* ["];
*/
