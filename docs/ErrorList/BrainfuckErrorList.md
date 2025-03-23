# Error / Warning List for Brainfuck

| Error Code |                 Name                  |                                      Description                                      |
|:----------:|:-------------------------------------:|:-------------------------------------------------------------------------------------:|
|   BFW01    |       Input Stream reached EOF        | A warning, usually you should not be surprised, because it means input stream ended.  |
|   BFE01    | Memory pointer forward out of bounds  | An error. When user want to move memory pointer out of zero bound (e.g. To Index -1). |
|   BFE02    | Memory pointer backward out of bounds |           An error. When user want to move memory pointer out of max size.            | 
|   BFE03    |             Unmatched '['             |                When interpreter only found '\[' without matching '\]'.                |
|   BFE04    |             Unmatched ']'             |                 When interpreter only found ']' without matching '['.                 |