VAR
    x, y : INTEGER.

BEGIN

    x := 2;
    y := 0;

    CASE x OF
        1: y := 10;
        2: y := 20;
        3, 4: y := 30;
    END;

    IF y == 20 THEN
        BEGIN
            x := x + 1
        END
    ELSE   
        BEGIN
            x := x - 1
        END;

    DISPLAY x;
    DISPLAY y

END.