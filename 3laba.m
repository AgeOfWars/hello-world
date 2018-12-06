clear all;
input = imread("x1.bmp");
output = imread("y1.bmp");
Xinput = input;
[xRows, xColumns] = size(input);
yRows = size(output)(1);
input = [input; ones(1,xColumns)];
xRows += 1;
input = double(input);

function invGr = grevilMethod(X)
    rows = size(X)(1);
    firstRow = X(1,:);
    eps = 10e-6;
    if (firstRow' * firstRow > eps)
        curMatrix = firstRow/(firstRow'*firstRow);
    else
        curMatrix = firstRow';
    endif
    A = [];

    for i = 2 : rows
        A = [A ; X(i-1,:)];
        curRow = X(i,:)';
        Z = eye(size(curMatrix)(1)) - curMatrix * A;
        R = curMatrix * curMatrix';
        zValue = curRow'*Z*curRow;
        rValue = curRow'*R*curRow;

        if (zValue > eps)
            curMatrix = curMatrix - Z * curRow * curRow' * curMatrix / zValue;
            curMatrix = [curMatrix Z * curRow / zValue];
        else
            curMatrix = curMatrix - R * curRow * curRow' * curMatrix / (1+rValue);
            curMatrix = [curMatrix R * curRow / (1+rValue)];
        endif

        invGr = curMatrix;
    endfor
endfunction

function invMat = invForDelta(X, delta)
    [rows, columns] = size(X);
    if (rows > columns)
        invMat = inv(X' * X + delta^2 * eye(columns)) * X';
    else
        invMat = X' * inv(X * X' + delta^2 * eye(rows));
    endif
endfunction

function invMura = muraPenrouseMethod(X)
    delta = 100;
    e = 10e-6;
    diff = 0;
    inv2 = invForDelta(X, delta);

    do
        inv1 = inv2;
        delta /= 2;
        inv2 = invForDelta(X, delta);
        diff = norm(inv1 - inv2);
    until diff < e;

    invMura = inv2;
endfunction

function A = getOperator(input, output, xInv)
    xRows = size(input)(1);
    yRows = size(output)(1);
    randomMatrix = rand(yRows, xRows);
    Z = eye(xRows) - input * xInv;
    A = double(output) * xInv + randomMatrix * Z;
endfunction


Agrevil = getOperator(input, output, grevilMethod(input));
Amura = getOperator(input, output, muraPenrouseMethod(input));

Ygrevil = uint8(Agrevil * input);
Ymura = uint8(Amura * input);

figure();
imshow(Ygrevil);
title("Grevil");
figure();
imshow(Ymura);
title("Mura-Penrouse");
figure();
imshow(Xinput);
title("input");
figure();
imshow(output);
title("output");
