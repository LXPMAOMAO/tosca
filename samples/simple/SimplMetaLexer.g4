    lexer grammar  SimplMetaLexer ;        MT_simpl :         CRSX_META_CHAR      'simpl'     [0-9]   *    ;     ET_simpl :         CRSX_START_EMBED_CHAR      'simpl:'     ->    pushMode (   CrsxEmbed )   ;     MT_assign :         CRSX_META_CHAR      'assign'     [0-9]   *    ;     ET_assign :         CRSX_START_EMBED_CHAR      'assign:'     ->    pushMode (   CrsxEmbed )   ;     MT_var_TOK :         CRSX_META_CHAR      'VAR'     [0-9]   *    ;     ET_var_TOK :         CRSX_START_EMBED_CHAR      'VAR:'     ->    pushMode (   CrsxEmbed )   ;     MT_assign_S1 :         CRSX_META_CHAR      'assign_S1'     [0-9]   *    ;     ET_assign_S1 :         CRSX_START_EMBED_CHAR      'assign_S1:'     ->    pushMode (   CrsxEmbed )   ;     MT_ass_TOK :         CRSX_META_CHAR      'ASS'     [0-9]   *    ;     ET_ass_TOK :         CRSX_START_EMBED_CHAR      'ASS:'     ->    pushMode (   CrsxEmbed )   ;     MT_exp :         CRSX_META_CHAR      'exp'     [0-9]   *    ;     ET_exp :         CRSX_START_EMBED_CHAR      'exp:'     ->    pushMode (   CrsxEmbed )   ;     MT_plus_TOK :         CRSX_META_CHAR      'PLUS'     [0-9]   *    ;     ET_plus_TOK :         CRSX_START_EMBED_CHAR      'PLUS:'     ->    pushMode (   CrsxEmbed )   ;     MT_number :         CRSX_META_CHAR      'number'     [0-9]   *    ;     ET_number :         CRSX_START_EMBED_CHAR      'number:'     ->    pushMode (   CrsxEmbed )   ;     MT_one_TOK :         CRSX_META_CHAR      'ONE'     [0-9]   *    ;     ET_one_TOK :         CRSX_START_EMBED_CHAR      'ONE:'     ->    pushMode (   CrsxEmbed )   ;     MT_zero_TOK :         CRSX_META_CHAR      'ZERO'     [0-9]   *    ;     ET_zero_TOK :         CRSX_START_EMBED_CHAR      'ZERO:'     ->    pushMode (   CrsxEmbed )   ;     ASS :         ':='     ;     PLUS :         '+'     ;     VAR :        [a-z]    ;     ONE :         '1'     ;     ZERO :         '0'     ;     fragment CRSX_META_CHAR :         '#'     ;     fragment CRSX_START_EMBED_CHAR :         '⟨'     ;     fragment CRSX_END_EMBED_CHAR :         '⟩'     ;   mode  CrsxEmbed ;     CRSX_EMBED_END :         CRSX_END_EMBED_CHAR     ->    popMode   ;    CRSX_EMBED_NESTED :         CRSX_START_EMBED_CHAR     ->    pushMode (   CrsxNestedEmbed )   ,    more  ;    CRSX_EMBEDDED :        .     ->    more   ;  mode  CrsxNestedEmbed ;     CRSX_NESTED_EMBED_END :         CRSX_END_EMBED_CHAR     ->    popMode   ,    more  ;    CRSX_NESTED_EMBED_NESTED :         CRSX_START_EMBED_CHAR     ->    pushMode (   CrsxNestedEmbed )   ,    more  ;    CRSX_NESTED_EMBEDDED :        .     ->    more   ;