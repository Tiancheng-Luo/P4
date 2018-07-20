#
# This file was automatically generated by MPLSTRIP.
# Source file: separatingcurves.tex
# Time stamp on source file: Wed Jul 18 16:01:44 2018
#
restart;
user_numpointscurves := [100]:
user_curves := [x*y]:
user_curvesfile := "untitled_sepcurves.tab":
returnvalue := 0:
findAllSeparatingCurves := proc()
    global user_f, user_numpoints, user_curves, rounded, user_numeric,
            returnvalue;
    local fp,k,r,numcurves,numcurveregions;

    user_numeric := true;
    rounded := true;
    numcurves := nops(user_curves);
    numcurveregions := nops(curveregions);
    fp := fopen(user_curvesfile, WRITE, TEXT);

    for r from 1 to numcurves do
        if type(user_curves[r], equation) or type(user_curves[r], `<=`) then
            user_curves[r] := lhs(user_curves[r])-rhs(user_curves[r]);
        end if;
        if CheckIfGoodPolynomial(user_curves[r]) = false then
            printf("The separating curve #%d: %a is not well-defined (not polynomial, or coefficients do not evaluate numerically.)\n", r, user_curves[r]);
            fclose(fp);
            returnvalue := 0;
            NULL;
            return;
        end if;
    end do;

    fprintf(fp, "p5\n%d %d %d %d\n", user_p, user_q, user_precision, numcurves);

    for r from 1 to numcurves do
        user_numpoints := user_numpointscurves[r];
        user_f := user_curves[r];
        if not findSeparatingCurve(fp) then
            fclose(fp);
            fclose(fopen(user_file, WRITE, TEXT)); # clear file
            returnvalue := 0;
            NULL;
            return;
        end if;
    end do;
    fclose(fp);
    returnvalue := 1;
    NULL;
end:
findSeparatingCurve := proc(fp)
    global user_x1, user_x2, user_y1, user_y2, u, v, user_f;
    local f, fU1, fV1, fU2, fV2, fC, z1, z2, s, L, j, d;

    f := user_f;    # store original curve
    d := ddeg(f, x, y);
    if d = 0 then
        fprintf(fp, "%d\n", 0)
    else
        L := coeff_degree2(gcff, x, y);
        fprintf(fp, "%d\n", d);
    end if;

    L := coeff_degree2(f, x, y);
    fprintf(fp, "%d", nops(L));
    for j from 1 to nops(L) do
        fprintf(fp, " %d %d %g", op(1, L[j]), op(2, L[j]), evalf(op(3,L[j])));
    end do;
    fprintf(fp, "\n");

    fU1 := subs(x = 1/z2^user_p, y = z1/z2^user_q, f);
    fU1 := norpoly2(fU1, z1, z2);
    fU1 := subs(z1=x, z2=y, numer(fU1));
    L := coeff_degree2(fU1, x, y);
    fprintf(fp, "%d", nops(L));
    for j from 1 to nops(L) do
        fprintf(fp, " %d %d %g", op(1, L[j]), op(2, L[j]), evalf(op(3,L[j])));
    end do;
    fprintf(fp, "\n");

    fU2 := subs(x = z1/z2^user_p, y = 1/z2^user_q, f);
    fU2 := norpoly2(fU2, z1,z2);
    fU2 := subs(z1=x, z2=y, numer(fU2));
    L := coeff_degree2(fU2, x, y);
    fprintf(fp, "%d", nops(L));
    for j from 1 to nops(L) do
        fprintf(fp, " %d %d %g", op(1, L[j]), op(2, L[j]), evalf(op(3,L[j])));
    end do;
    fprintf(fp, "\n");

    fV1 := subs(x = -1/z2^user_p, y = z1/z2^user_q, f);
    fV1 := norpoly2(fV1, z1,z2);
    fV1 := subs(z1=x, z2=y, numer(fV1));
    L := coeff_degree2(fV1, x, y);
    fprintf(fp, "%d", nops(L));
    for j from 1 to nops(L) do
        fprintf(fp, " %d %d %g", op(1, L[j]), op(2, L[j]), evalf(op(3,L[j])));
    end do;
    fprintf(fp, "\n");

    fV2 := subs(x = z1/z2^user_p, y = -1/z2^user_q, f);
    fV2 := norpoly2(fV2, z1,z2);
    fV2 := subs(z1=x, z2=y, numer(fV2));
    L := coeff_degree2(fV2, x, y);
    fprintf(fp, "%d", nops(L));
    for j from 1 to nops(L) do
        fprintf(fp, " %d %d %g", op(1, L[j]), op(2, L[j]), evalf(op(3,L[j])));
    end do;
    fprintf(fp, "\n");

    if user_p = 1 and user_q = 1 then

        user_f := f;
        user_x1 := -1.0;
        user_x2 := 1.0;
        user_y1 := -1.0;
        user_y2 := 1.0;
        u := x;
        v := y;
        printf("Chart R2: ");
        if not findSeparatingCurveInChart(fp) then
            return false;
        end if;

        user_f := fU1;
        user_x1 := -1.0;
        user_x2 := 1.0;
        user_y1 := 0.0;
        user_y2 := 1.0;
        u := x;
        v := y;
        printf("Chart U1: ");
        if not findSeparatingCurveInChart(fp) then
            return false;
        end if;

        user_f := fU2;
        user_x1 := -1.0;
        user_x2 := 1.0;
        user_y1 := 0.0;
        user_y2 := 1.0;
        u := x;
        v := y;
        printf("Chart U2: ");
        if not findSeparatingCurveInChart(fp) then
            return false;
        end if;

        user_f := fU1;
        user_x1 := -1.0;
        user_x2 := 1.0;
        user_y1 := -1.0;
        user_y2 := 0.0;
        u := x;
        v := y;
        printf("Chart V1': ");
        if not findSeparatingCurveInChart(fp) then
            return false;
        end if;

        user_f := fU2;
        user_x1 := -1.0;
        user_x2 := 1.0;
        user_y1 := -1.0;
        user_y2 := 0.0;
        u := x;
        v := y;
        printf("Chart V2': ");
        if not findSeparatingCurveInChart(fp) then
            return false;
        end if;

    else
        fC := subs(x=U/s^user_p, y = V/s^user_q, f);
        fC := norpoly3(fC, s, U, V);
        fC := subs(s=x, numer(fC));

        L := coeff_degree3(fC, x, U, V);
        fprintf(fp, "%d", nops(L));
        for j from 1 to nops(L) do
            fprintf(fp, " %d %d %d %g", op(1, L[j]), op(2, L[j]), op(3, L[j]),
                    evalf(op(4,L[j])));
        end do;
        fprintf(fp, "\n");

        user_f := eval(f,{x=U,y=V});
        user_x1 := 0.0;
        user_x2 := 1.0;
        user_y1 := 0.0;
        user_y2 := evalf(2*Pi);
        u := x*cos(y);
        v := x*sin(y);
        printf("Chart R2 (Lyapunov-weights): ");
        if not findSeparatingCurveInChart(fp) then
            return false;
        end if;

        user_x1 := 0.0;
        user_x2 := 1.0;
        user_y1 := evalf(-Pi/4);
        user_y2 := evalf(Pi/4);
        user_f := fC;
        u := cos(y);
        v := sin(y);
        printf("Chart U1 (Lyapunov-weights): ");
        if not findSeparatingCurveInChart(fp) then
            return false;
        end if;

        user_x1 := 0.0;
        user_x2 := 1.0;
        user_y1 := evalf(Pi/4);
        user_y2 := evalf(Pi-Pi/4);
        user_f := fC;
        u := cos(y);
        v := sin(y);
        printf("Chart U2 (Lyapunov-weights): ");
        if not findSeparatingCurveInChart(fp) then
            return false;
        end if;

        user_x1 := 0.0;
        user_x2 := 1.0;
        user_y1 := evalf(Pi-Pi/4);
        user_y2 := evalf(Pi+Pi/4);
        user_f := fC;
        u := cos(y);
        v := sin(y);
        printf("Chart V1 (Lyapunov-weights): ");
        if not findSeparatingCurveInChart(fp) then
            return false;
        end if;

        user_x1 := 0.0;
        user_x2 := 1.0;
        user_y1 := evalf(-Pi+Pi/4);
        user_y2 := evalf(-Pi/4);
        user_f := fC;
        u := cos(y);
        v := sin(y);
        printf("Chart V2 (Lyapunov-weights): ");
        if not findSeparatingCurveInChart(fp) then
            return false;
        end if;
    end if;
    return true;
end:
findSeparatingCurveInChart := proc(fp)
    global u, v, returnvalue, facs, fac, user_f, user_numpoints;
    local m, n, P, k, i, j, s, f, CURVES_ARRAY_TO_LISTLIST, SimplifyPlot;

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
        printf("%a: Level curve is a constant.\n", user_f);
        fprintf(fp, "%d\n", 0);
        return true;
    else
        m := round(sqrt(user_numpoints)) + 1:
        n := m:
        facs := factors(subs({U=u,V=v},user_f))[2];
        P := map(fac -> plots[implicitplot](fac,
                    x=user_x1..user_x2,y=user_y1..user_y2,axes=none,
                    view=[user_x1..user_x2,user_y1..user_y2],grid=[m,n],
                    color=black), facs):
        P := map(SimplifyPlot, P);
        P := map(fac -> op(select(pt -> type(pt,list), [op(op(1,fac))])), P):
        k := 0:
        for i from 1 to nops(P) do
            k := k + nops(P[i]);
        end do;
        fprintf(fp, "%d\n", k);
        k := 0;
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
        printf("%a: %d points returned.\n", user_f, k);
    end if;
    returnvalue := 0;
    return true;
end:
save(returnvalue, findAllSeparatingCurves, findSeparatingCurve,
        findSeparatingCurveInChart, "separatingcurves.m");