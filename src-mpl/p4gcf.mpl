#
# This file was automatically generated by MPLSTRIP.
# Source file: p4gcf.tex
# Time stamp on source file: Tue Oct 24 14:36:22 2017
#
restart;
user_numpoints := 100:
u := x:
v := y:
user_f := x*y:
user_file := "untitled_gcf.tab":
user_x1 := -1:
user_x2 := 1:
user_y1 := -1:
user_y2 := 1:
returnvalue := 0:
FindSingularities := proc()
    global  user_numpoints, u, v, user_f, user_file,
            user_x1, user_x2, user_y1, user_y2, returnvalue, facs, fac;
    local   m, n, P, k, i, j, fp, s, f, SimplifyPlot, CURVES_ARRAY_TO_LISTLIST;

    CURVES_ARRAY_TO_LISTLIST := proc()
        local ss, k;
        ss := [$1..nargs];
        for k from 1 to nargs do
            ss[k] := args[k];
            if type(ss[k], Array) then
                if ArrayNumDims(ss[k]) = 2 then
                    ss[k] := convert(ss[k], listlist);
                elif ArrayNumDims(ss[k]) = 1 then
                    ss[k] := convert(ss[k], list);
                end if;
            end if;
        end do;
        CURVES(op(ss));
    end proc;
    
    SimplifyPlot := P -> eval(P, CURVES=CURVES_ARRAY_TO_LISTLIST);
    
    returnvalue := 1;
    if degree(user_f, [x,y,U,V]) <= 0 then
        printf("Greatest common factor is a constant.\n");
        fp = fopen(user_file, WRITE, TEXT);
        fclose(fp);
        k:=0;
    else
        m := round(sqrt(user_numpoints)) + 1:
        n := m:
        facs := factors(subs({U=u,V=v},user_f))[2];
        P := map(fac -> plots[implicitplot](fac,
            x=user_x1..user_x2,y=user_y1..user_y2,axes=none,
            view=[user_x1..user_x2,user_y1..user_y2],grid=[m,n],color=black), facs):
        P := map(SimplifyPlot, P);
        P := map(fac -> op(select(pt -> type(pt,list), [op(op(1,fac))])), P):
        k := 0:
        fp := fopen(user_file, WRITE, TEXT);
        for i from 1 to nops(P) do
            for j from 1 to nops(P[i]) do
                s := sprintf("%g %g ", P[i,j,1], P[i,j,2]);
                fprintf(fp, "%s", s);
                k := k+1;
            end do;
            if i <> nops(P) then
                fprintf(fp, ",\n");
            else
                fprintf(fp, "\n");
            end if;
        end do:
        fclose(fp);
    end if;
    printf("%d points returned.\n", k);
    returnvalue := 0;
    NULL;
    end:
save(user_numpoints, u, v, user_f, user_file, user_x1, user_x2, user_y1, user_y2,
      returnvalue, FindSingularities, "p4gcf.m");
