#include "Group.h"
#include <unistd.h>

//the operations could be optimized as templates//

//------G1 ops------//

void G1Op::add_plain_aff(G1_Affine_Coordinates &c,
                         const G1_Affine_Coordinates &a,
                         const G1_Affine_Coordinates_Plain &b)
{
    Share dx;
    Share m, mm;
    sub_plain(m, a.y, b.y);
    sub_plain(dx, a.x, b.x);

    div_inplace(m, dx); // m = (y1-y2)/(x1-x2)
    sqr(mm, m);         // mm = m^2

    sub_inplace(mm, a.x);
    sub_plain(c.x, mm, b.x); // x3 = m^2-x1-x2

    sub(c.y, a.x, c.x);
    mul_inplace(c.y, m);
    sub_inplace(c.y, a.y); // y3 = m(x1-x3)-y1
}

void G1Op::add_plain_aff_inplace(G1_Affine_Coordinates &c,
                                 const G1_Affine_Coordinates_Plain &a)
{
    G1_Affine_Coordinates tmp;
    add_plain_aff(tmp, c, a);
    c = tmp;
}
void G1Op::add_aff(G1_Affine_Coordinates &c,
                   const G1_Affine_Coordinates &a,
                   const G1_Affine_Coordinates &b)
{
    Share dx;
    Share m, mm;
    sub(m, a.y, b.y);
    sub(dx, a.x, b.x);

    div_inplace(m, dx); // m = (y1-y2)/(x1-x2)
    sqr(mm, m);         // mm = m^2

    sub_inplace(mm, a.x);
    sub(c.x, mm, b.x); // x3 = m^2-x1-x2

    sub(c.y, a.x, c.x);
    mul_inplace(c.y, m);
    sub_inplace(c.y, a.y); // y3 = m(x1-x3)-y1
}

void G1Op::add_aff_inplace(G1_Affine_Coordinates &c, const G1_Affine_Coordinates &a)
{
    G1_Affine_Coordinates tmp;
    add_aff(tmp, c, a);
    c = tmp;
}

void G1Op::add_plain_jac(G1_Jacobian_Coordinates &c,
                         const G1_Jacobian_Coordinates &a,
                         const G1_Jacobian_Coordinates_Plain &b)
{
    gfp bZ3;
    Share S1, S2, U1, U2;
    sqr(S2, a.Z);
    bZ3 = b.Z * b.Z;

    mul_plain(U2, S2, b.X);  //U2 = X2*Z1^2
    mul_plain(U1, a.X, bZ3); //U1 = X1*Z2^2

    Share S;
    mul_plain(S, S2, b.Y);
    mul(S2, S, a.Z); // S2 = Y2*Z1^3

    bZ3 = bZ3 * b.Z;
    mul_plain(S1, a.Y, bZ3); // S1 = Y1*Z2^3

    Share R, H, V, G;
    sub(R, S1, S2); //R = S1-S2
    sub(H, U1, U2); // H = U1-U2

    sqr(V, H);
    mul(G, V, H);       //G = H^3
    mul_inplace(V, U1); // V = U1*H^2

    mul_plain(c.Z, a.Z, b.Z);
    mul_inplace(c.Z, H); // Z3 = Z1*Z2*H

    Share tmp;
    sqr(tmp, R);

    add(c.X, tmp, G);
    sub_inplace(c.X, V);
    sub_inplace(c.X, V); //X3 = R^2+G-2V

    mul(tmp, S1, G);
    sub(c.Y, V, c.X);
    mul_inplace(c.Y, R);
    sub_inplace(c.Y, tmp); //Y3 = R*(V-X3)-S1*G
}

void G1Op::add_plain_jac_inplace(G1_Jacobian_Coordinates &c,
                                 const G1_Jacobian_Coordinates_Plain &a)
{
    G1_Jacobian_Coordinates tmp;
    add_plain_jac(tmp, c, a);
    c = tmp;
}
void G1Op::add_jac(G1_Jacobian_Coordinates &c,
                   const G1_Jacobian_Coordinates &a,
                   const G1_Jacobian_Coordinates &b)
{
    Share S1, S2, U1, U2;
    sqr(S2, a.Z);
    sqr(S1, b.Z);

    mul(U2, b.X, S2); //U2 = X2*Z1^2
    mul(U1, a.X, S1); //U1 = X1*Z2^2

    mul_inplace(S2, b.Y);
    mul_inplace(S2, a.Z); // S2 = Y2*Z1^3

    mul_inplace(S1, a.Y);
    mul_inplace(S1, b.Z); // S1 = Y1*Z2^3

    Share R, H, V, G;
    sub(R, S1, S2); //R = S1-S2
    sub(H, U1, U2); // H = U1-U2

    sqr(V, H);
    mul(G, V, H);       //G = H^3
    mul_inplace(V, U1); // V = U1*H^2

    mul(c.Z, a.Z, b.Z);
    mul_inplace(c.Z, H); // Z3 = Z1*Z2*H

    Share tmp;
    sqr(tmp, R);

    add(c.X, tmp, G);
    sub_inplace(c.X, V);
    sub_inplace(c.X, V); //X3 = R^2+G-2V

    mul(tmp, S1, G);
    sub(c.Y, V, c.X);
    mul_inplace(c.Y, R);
    sub_inplace(c.Y, tmp); //Y3 = R*(V-X3)-S1*G
}

void G1Op::add_jac_inplace(G1_Jacobian_Coordinates &c,
                           const G1_Jacobian_Coordinates &a)
{
    G1_Jacobian_Coordinates tmp;
    add_jac(tmp, c, a);
    c = tmp;
}

void G1Op::add_plain_proj(G1_Projective_Coordinates &c,
                          const G1_Projective_Coordinates &a,
                          const G1_Projective_Coordinates_Plain &b)
{
    /* could optimize muls with same multiplier *
     * could optimize the RAM used              */

    Share U1, U2, S1, S2;
    mul_plain(S2, a.Z, b.Y); //S2 = Y2 * Z1
    mul_plain(S1, a.Y, b.Z); //S1 = Y1 * Z2
    mul_plain(U2, a.Z, b.X); //U2 = X2 * Z1
    mul_plain(U1, a.X, b.Z); //U1 = X1 * Z2

    Share T, H, R;
    sub(R, S1, S2);         //R = S1 - S2
    sub(H, U1, U2);         //H = U1 - U2
    mul_plain(T, a.Z, b.Z); //T = Z1 * Z2

    Share G, V, W;
    Share R2, H2;

    sqr(H2, H);     //H2 = H^2
    mul(V, U1, H2); //V = U1*H^2

    mul(G, H2, H); // G = H^3

    sqr(R2, R); //R2 = R^2
    mul(W, R2, T);
    add_inplace(W, G);
    sub_inplace(W, V);
    sub_inplace(W, V); // W = R^2*T + G - 2V

    mul(c.Z, T, G); //Z3 = T * G

    Share tmp;
    mul(tmp, S1, G);
    sub(c.Y, V, W);
    mul_inplace(c.Y, R);
    sub_inplace(c.Y, tmp); // Y3 = R*(V-W)-S1*G

    mul(c.X, H, W); // X3 = H * W
}

void G1Op::add_plain_proj_inplace(G1_Projective_Coordinates &c,
                                  const G1_Projective_Coordinates_Plain &a)
{
    G1_Projective_Coordinates tmp;
    add_plain_proj(tmp, c, a);
    c = tmp;
}

void G1Op::add_proj(G1_Projective_Coordinates &c,
                    const G1_Projective_Coordinates &a,
                    const G1_Projective_Coordinates &b)
{
    /* could optimize muls with same multiplier *
     * could optimize the RAM used              */

    Share U1, U2, S1, S2;
    mul(S2, b.Y, a.Z); //S2 = Y2 * Z1
    mul(S1, a.Y, b.Z); //S1 = Y1 * Z2
    mul(U2, b.X, a.Z); //U2 = X2 * Z1
    mul(U1, a.X, b.Z); //U1 = X1 * Z2

    Share T, H, R;
    sub(R, S1, S2);   //R = S1 - S2
    sub(H, U1, U2);   //H = U1 - U2
    mul(T, a.Z, b.Z); //T = Z1 * Z2

    Share G, V, W;
    Share R2, H2;

    sqr(H2, H);     //H2 = H^2
    mul(V, U1, H2); //V = U1*H^2

    mul(G, H2, H); // G = H^3

    sqr(R2, R); //R2 = R^2
    mul(W, R2, T);
    add_inplace(W, G);
    sub_inplace(W, V);
    sub_inplace(W, V); // W = R^2*T + G - 2V

    mul(c.Z, T, G); //Z3 = T * G

    Share tmp;
    mul(tmp, S1, G);
    sub(c.Y, V, W);
    mul_inplace(c.Y, R);
    sub_inplace(c.Y, tmp); // Y3 = R*(V-W)-S1*G

    mul(c.X, H, W); // X3 = H * W
}

void G1Op::add_proj_inplace(G1_Projective_Coordinates &c,
                            const G1_Projective_Coordinates &a)
{
    G1_Projective_Coordinates tmp;
    add_proj(tmp, c, a);
    c = tmp;
}
void G1Op::reveal_and_print(const G1_Affine_Coordinates ac)
{
    cout << "x: " << endl;
    OnlineOp::reveal_and_print({ac.x});
    cout << "y: " << endl;
    OnlineOp::reveal_and_print({ac.y});
}
template <typename T>
void G1Op::reveal_and_print(const T p)
{
    cout << "X: " << endl;
    OnlineOp::reveal_and_print({p.X});
    cout << "Y: " << endl;
    OnlineOp::reveal_and_print({p.Y});
    cout << "Z: " << endl;
    OnlineOp::reveal_and_print({p.Z});
}

//------test--------//
void str_to_gfp(gfp &ans, const string &str)
{
    bigint bn(str.c_str(), 10);
    to_gfp(ans, bn);
}

void G1Op::G1_test_add_aff()
{
    cout << "============================== BEG " << __FUNCTION__ << " ==============================" << endl;
    vector<string> point1(2), point2(2);
    point1[0] = "1295728164626534415618781265254580659926272354843985135401485454504777372546472596033371574568093826491865410771001";
    point1[1] = "3839565232484830821203974136028462429409860797185329370952032679694793040839037122415038227769061259923986030367015";

    point2[0] = "613192939246165145697364032654702068835570358685595826913421400569692251883551998352417059167240540423077723627092";
    point2[1] = "1939834878592542483572653782999203248604368035458557492969812954708733328780181922866866198381425602016362784974612";

    gfp g1x, g1y, g2x, g2y;
    str_to_gfp(g1x, point1[0]);
    str_to_gfp(g1y, point1[1]);
    str_to_gfp(g2x, point2[0]);
    str_to_gfp(g2y, point2[1]);

    G1_Affine_Coordinates_Plain acp(g2x, g2y);

    vector<G1_Affine_Coordinates> ac(2);
    get_inputs(0, ac[0].x, g1x);
    get_inputs(0, ac[0].y, g1y);

    get_inputs(1, ac[1].x, g2x);
    get_inputs(1, ac[1].y, g2y);
    cout << "--------test add_affine--------" << endl;
    G1_Affine_Coordinates res(ac[0]);
    reveal_and_print(res);

    add_aff_inplace(res, ac[1]);
    reveal_and_print(res);
    cout << "--------test add_plain_affine--------" << endl;

    res = ac[0];
    add_plain_aff_inplace(res, acp);
    reveal_and_print(res);
}

void G1Op::G1_test_add_jac()
{
    cout << "============================== BEG " << __FUNCTION__ << " ==============================" << endl;
    vector<string> point1(3), point2(3);
    point1[0] = "3685416753713387016781088315183077757961620795782546409894578378688607592378376318836054947676345821548104185464507";
    point1[1] = "1339506544944476473020471379941921221584933875938349620426543736416511423956333506472724655353366534992391756441569";
    point1[2] = "1";

    point2[0] = "506495629725590588476438736938928437216878988401200917968428110806855219037361470594787775621162927277631758167243";
    point2[1] = "3215784584818841779393380451963501913485474208604528267666791755940205624806737197312591247682567521662969376111781";
    point2[2] = "2679013089888952946040942759883842443169867751876699240853087472833022847912667012945449310706733069984783512883138";

    gfp g1x, g1y, g1z, g2x, g2y, g2z;
    str_to_gfp(g1x, point1[0]);
    str_to_gfp(g1y, point1[1]);
    str_to_gfp(g1z, point1[2]);
    str_to_gfp(g2x, point2[0]);
    str_to_gfp(g2y, point2[1]);
    str_to_gfp(g2z, point2[2]);

    G1_Jacobian_Coordinates_Plain jcp(g2x, g2y, g2z);

    vector<G1_Jacobian_Coordinates> jc(2);
    get_inputs(0, jc[0].X, g1x);
    get_inputs(0, jc[0].Y, g1y);
    get_inputs(0, jc[0].Z, g1z);

    get_inputs(1, jc[1].X, g2x);
    get_inputs(1, jc[1].Y, g2y);
    get_inputs(1, jc[1].Z, g2z);
    cout << "--------test add_jacobian--------" << endl;
    G1_Jacobian_Coordinates res(jc[0]);
    reveal_and_print(res);

    add_jac_inplace(res, jc[1]);
    reveal_and_print(res);
    cout << "--------test add_plain_jacobian--------" << endl;

    res = jc[0];
    reveal_and_print(res);
    add_plain_jac_inplace(res, jcp);
    reveal_and_print(res);
}

void G1Op::G1_test_add_proj()
{
    cout << "============================== BEG " << __FUNCTION__ << " ==============================" << endl;
    vector<string> point1(3), point2(3);
    point1[0] = "3685416753713387016781088315183077757961620795782546409894578378688607592378376318836054947676345821548104185464507";
    point1[1] = "1339506544944476473020471379941921221584933875938349620426543736416511423956333506472724655353366534992391756441569";
    point1[2] = "1";

    point2[0] = "904218658502494312590159247105554874787512476566126303635147587402923829641584622770449558420966780868207684298157";
    point2[1] = "3215784584818841779393380451963501913485474208604528267666791755940205624806737197312591247682567521662969376111781";
    point2[2] = "645039760431336131708729805154580111675108263123058424684373273230141681090362840405422204836635344492771939114786";

    gfp g1x, g1y, g1z, g2x, g2y, g2z;
    str_to_gfp(g1x, point1[0]);
    str_to_gfp(g1y, point1[1]);
    str_to_gfp(g1z, point1[2]);
    str_to_gfp(g2x, point2[0]);
    str_to_gfp(g2y, point2[1]);
    str_to_gfp(g2z, point2[2]);

    G1_Projective_Coordinates_Plain pcp(g2x, g2y, g2z);

    vector<G1_Projective_Coordinates> pc(2);
    get_inputs(0, pc[0].X, g1x);
    get_inputs(0, pc[0].Y, g1y);
    get_inputs(0, pc[0].Z, g1z);

    get_inputs(1, pc[1].X, g2x);
    get_inputs(1, pc[1].Y, g2y);
    get_inputs(1, pc[1].Z, g2z);
    cout << "--------test add_projective--------" << endl;
    G1_Projective_Coordinates res(pc[0]);
    reveal_and_print(res);

    add_proj_inplace(res, pc[1]);
    reveal_and_print(res);
    cout << "--------test add_plain_projective--------" << endl;

    res = pc[0];
    add_plain_proj_inplace(res, pcp);
    reveal_and_print(res);
}

//------G2 ops------//

void G2Op::add_plain_aff(G2_Affine_Coordinates &c,
                         const G2_Affine_Coordinates &a,
                         const G2_Affine_Coordinates_Plain &b)
{
    Complex dx;
    Complex m, mm;
    sub_plain(m, a.y, b.y);
    sub_plain(dx, a.x, b.x);

    div_inplace(m, dx); // m = (y1-y2)/(x1-x2)
    sqr(mm, m);         // mm = m^2

    sub_inplace(mm, a.x);
    sub_plain(c.x, mm, b.x); // x3 = m^2-x1-x2

    sub(c.y, a.x, c.x);
    mul_inplace(c.y, m);
    sub_inplace(c.y, a.y); // y3 = m(x1-x3)-y1
}

void G2Op::add_plain_aff_inplace(G2_Affine_Coordinates &c,
                                 const G2_Affine_Coordinates_Plain &a)
{
    G2_Affine_Coordinates tmp;
    add_plain_aff(tmp, c, a);
    c = tmp;
}
void G2Op::add_aff(G2_Affine_Coordinates &c,
                   const G2_Affine_Coordinates &a,
                   const G2_Affine_Coordinates &b)
{
    Complex dx;
    Complex m, mm;
    sub(m, a.y, b.y);
    sub(dx, a.x, b.x);

    div_inplace(m, dx); // m = (y1-y2)/(x1-x2)
    sqr(mm, m);         // mm = m^2

    sub_inplace(mm, a.x);
    sub(c.x, mm, b.x); // x3 = m^2-x1-x2

    sub(c.y, a.x, c.x);
    mul_inplace(c.y, m);
    sub_inplace(c.y, a.y); // y3 = m(x1-x3)-y1
}

void G2Op::add_aff_inplace(G2_Affine_Coordinates &c,
                           const G2_Affine_Coordinates &a)
{
    G2_Affine_Coordinates tmp;
    add_aff(tmp, c, a);
    c = tmp;
}

void mul_complex_plain(Complex_Plain &c, const Complex_Plain &a, const Complex_Plain &b)
{
    gfp r2, i2;
    r2 = a.real * b.real;
    i2 = a.imag * b.imag;
    c.real = r2 - i2;

    r2 = a.real * b.imag;
    i2 = a.imag * b.real;
    c.imag = r2 + i2;
}

void mul_complex_plain_inplace(Complex_Plain &c, const Complex_Plain &a)
{
    Complex_Plain tmp;
    mul_complex_plain(tmp, c, a);
    c = tmp;
}

void G2Op::add_plain_jac(G2_Jacobian_Coordinates &c,
                         const G2_Jacobian_Coordinates &a,
                         const G2_Jacobian_Coordinates_Plain &b)
{
    Complex_Plain bZ3;
    Complex S1, S2, U1, U2;
    sqr(S2, a.Z);
    mul_complex_plain(bZ3, b.Z, b.Z);

    mul_plain(U2, S2, b.X);  //U2 = X2*Z1^2
    mul_plain(U1, a.X, bZ3); //U1 = X1*Z2^2

    Complex S;
    mul_plain(S, S2, b.Y);
    mul(S2, S, a.Z); // S2 = Y2*Z1^3

    mul_complex_plain_inplace(bZ3, b.Z);

    mul_plain(S1, a.Y, bZ3); // S1 = Y1*Z2^3

    Complex R, H, V, G;
    sub(R, S1, S2); //R = S1-S2
    sub(H, U1, U2); // H = U1-U2

    sqr(V, H);
    mul(G, V, H);       //G = H^3
    mul_inplace(V, U1); // V = U1*H^2

    mul_plain(c.Z, a.Z, b.Z);
    mul_inplace(c.Z, H); // Z3 = Z1*Z2*H

    Complex tmp;
    sqr(tmp, R);

    add(c.X, tmp, G);
    sub_inplace(c.X, V);
    sub_inplace(c.X, V); //X3 = R^2+G-2V

    mul(tmp, S1, G);
    sub(c.Y, V, c.X);
    mul_inplace(c.Y, R);
    sub_inplace(c.Y, tmp); //Y3 = R*(V-X3)-S1*G
}

void G2Op::add_plain_jac_inplace(G2_Jacobian_Coordinates &c,
                                 const G2_Jacobian_Coordinates_Plain &a)
{
    G2_Jacobian_Coordinates tmp;
    add_plain_jac(tmp, c, a);
    c = tmp;
}

void G2Op::add_jac(G2_Jacobian_Coordinates &c,
                   const G2_Jacobian_Coordinates &a,
                   const G2_Jacobian_Coordinates &b)
{
    Complex S1, S2, U1, U2;
    sqr(S2, a.Z);
    sqr(S1, b.Z);

    mul(U2, b.X, S2); //U2 = X2*Z1^2
    mul(U1, a.X, S1); //U1 = X1*Z2^2

    mul_inplace(S2, b.Y);
    mul_inplace(S2, a.Z); // S2 = Y2*Z1^3

    mul_inplace(S1, a.Y);
    mul_inplace(S1, b.Z); // S1 = Y1*Z2^3

    Complex R, H, V, G;
    sub(R, S1, S2); //R = S1-S2
    sub(H, U1, U2); // H = U1-U2

    sqr(V, H);
    mul(G, V, H);       //G = H^3
    mul_inplace(V, U1); // V = U1*H^2

    mul(c.Z, a.Z, b.Z);
    mul_inplace(c.Z, H); // Z3 = Z1*Z2*H

    Complex tmp;
    sqr(tmp, R);

    add(c.X, tmp, G);
    sub_inplace(c.X, V);
    sub_inplace(c.X, V); //X3 = R^2+G-2V

    mul(tmp, S1, G);
    sub(c.Y, V, c.X);
    mul_inplace(c.Y, R);
    sub_inplace(c.Y, tmp); //Y3 = R*(V-X3)-S1*G
}

void G2Op::add_jac_inplace(G2_Jacobian_Coordinates &c,
                           const G2_Jacobian_Coordinates &a)
{
    G2_Jacobian_Coordinates tmp;
    add_jac(tmp, c, a);
    c = tmp;
}

void G2Op::add_plain_proj(G2_Projective_Coordinates &c,
                          const G2_Projective_Coordinates &a,
                          const G2_Projective_Coordinates_Plain &b)
{
    /* could optimize muls with same multiplier *
     * could optimize the RAM used              */

    Complex U1, U2, S1, S2;
    mul_plain(S2, a.Z, b.Y); //S2 = Y2 * Z1
    mul_plain(S1, a.Y, b.Z); //S1 = Y1 * Z2
    mul_plain(U2, a.Z, b.X); //U2 = X2 * Z1
    mul_plain(U1, a.X, b.Z); //U1 = X1 * Z2

    Complex T, H, R;
    sub(R, S1, S2);         //R = S1 - S2
    sub(H, U1, U2);         //H = U1 - U2
    mul_plain(T, a.Z, b.Z); //T = Z1 * Z2

    Complex G, V, W;
    Complex R2, H2;

    sqr(H2, H);     //H2 = H^2
    mul(V, U1, H2); //V = U1*H^2

    mul(G, H2, H); // G = H^3

    sqr(R2, R); //R2 = R^2
    mul(W, R2, T);
    add_inplace(W, G);
    sub_inplace(W, V);
    sub_inplace(W, V); // W = R^2*T + G - 2V

    mul(c.Z, T, G); //Z3 = T * G

    Complex tmp;
    mul(tmp, S1, G);
    sub(c.Y, V, W);
    mul_inplace(c.Y, R);
    sub_inplace(c.Y, tmp); // Y3 = R*(V-W)-S1*G

    mul(c.X, H, W); // X3 = H * W
}

void G2Op::add_plain_proj_inplace(G2_Projective_Coordinates &c,
                                  const G2_Projective_Coordinates_Plain &a)
{
    G2_Projective_Coordinates tmp;
    add_plain_proj(tmp, c, a);
    c = tmp;
}

void G2Op::add_proj(G2_Projective_Coordinates &c,
                    const G2_Projective_Coordinates &a,
                    const G2_Projective_Coordinates &b)
{
    /* could optimize muls with same multiplier *
     * could optimize the RAM used              */

    Complex U1, U2, S1, S2;
    mul(S2, b.Y, a.Z); //S2 = Y2 * Z1
    mul(S1, a.Y, b.Z); //S1 = Y1 * Z2
    mul(U2, b.X, a.Z); //U2 = X2 * Z1
    mul(U1, a.X, b.Z); //U1 = X1 * Z2

    Complex T, H, R;
    sub(R, S1, S2);   //R = S1 - S2
    sub(H, U1, U2);   //H = U1 - U2
    mul(T, a.Z, b.Z); //T = Z1 * Z2

    Complex G, V, W;
    Complex R2, H2;

    sqr(H2, H);     //H2 = H^2
    mul(V, U1, H2); //V = U1*H^2

    mul(G, H2, H); // G = H^3

    sqr(R2, R); //R2 = R^2
    mul(W, R2, T);
    add_inplace(W, G);
    sub_inplace(W, V);
    sub_inplace(W, V); // W = R^2*T + G - 2V

    mul(c.Z, T, G); //Z3 = T * G

    Complex tmp;
    mul(tmp, S1, G);
    sub(c.Y, V, W);
    mul_inplace(c.Y, R);
    sub_inplace(c.Y, tmp); // Y3 = R*(V-W)-S1*G

    mul(c.X, H, W); // X3 = H * W
}

void G2Op::add_proj_inplace(G2_Projective_Coordinates &c,
                            const G2_Projective_Coordinates &a)
{
    G2_Projective_Coordinates tmp;
    add_proj(tmp, c, a);
    c = tmp;
}

void G2Op::reveal_and_print(const G2_Affine_Coordinates ac)
{
    cout << "x: " << endl;
    OnlineOp::reveal_and_print({ac.x});
    cout << "y: " << endl;
    OnlineOp::reveal_and_print({ac.y});
}
template <typename T>
void G2Op::reveal_and_print(const T p)
{
    cout << "X: " << endl;
    OnlineOp::reveal_and_print({p.X});
    cout << "Y: " << endl;
    OnlineOp::reveal_and_print({p.Y});
    cout << "Z: " << endl;
    OnlineOp::reveal_and_print({p.Z});
}

//------test--------//
void G2Op::G2_test_add_aff()
{
    cout << "============================== BEG " << __FUNCTION__ << " ==============================" << endl;
    vector<vector<string>> point1(2, vector<string>(2)), point2(2, vector<string>(2));
    point1[0][0] = "2480630520447434459745452474224673571710954591933549968516737967418913165838049948371936430061723054690854233264184";
    point1[0][1] = "3644469270405057610286350249996588958371264130321941529490169899587077020220364734985787921562794068692573854501481";
    point1[1][0] = "1331800231700325429192215098048620074430720732324609804590563532772919675450158559950696523029296904480866114179517";
    point1[1][1] = "588900668981719074254304107998469524184236332635267773028573814801942986454206480288099393514359687714746219871656";

    point2[0][0] = "2536380239466922628470206732310665688174601869351732874387718117230652721298290649516310514212623300628740169159805";
    point2[0][1] = "680798372151742087638344386270749882234991936103419105455749081205553938870445287693249697293445242032096602347154";
    point2[1][0] = "2496542435097006551520642381398918251422033459118315432610085277783037480240104245041384280741212331277150387647414";
    point2[1][1] = "2036988673135752780119671948264754463514104930634625481515583855300300477160332414971334065417292937999677606499282";

    Complex_Plain g1x, g1y, g2x, g2y;
    str_to_gfp(g1x.real, point1[0][0]);
    str_to_gfp(g1x.imag, point1[0][1]);
    str_to_gfp(g1y.real, point1[1][0]);
    str_to_gfp(g1y.imag, point1[1][1]);

    str_to_gfp(g2x.real, point2[0][0]);
    str_to_gfp(g2x.imag, point2[0][1]);
    str_to_gfp(g2y.real, point2[1][0]);
    str_to_gfp(g2y.imag, point2[1][1]);

    G2_Affine_Coordinates_Plain acp(g2x, g2y);

    vector<G2_Affine_Coordinates> ac(2);
    get_inputs(0, ac[0].x, g1x);
    get_inputs(0, ac[0].y, g1y);

    get_inputs(1, ac[1].x, g2x);
    get_inputs(1, ac[1].y, g2y);
    cout << "--------test add_affine--------" << endl;
    G2_Affine_Coordinates res(ac[0]);
    reveal_and_print(res);
    cout << "*******\n";
    reveal_and_print(ac[1]);
    cout << "*******\n";
    add_aff_inplace(res, ac[1]);
    reveal_and_print(res);
    cout << "--------test add_plain_affine--------" << endl;

    res = ac[0];
    add_plain_aff_inplace(res, acp);
    reveal_and_print(res);
}

void G2Op::G2_test_add_jac()
{
    cout << "============================== BEG " << __FUNCTION__ << " ==============================" << endl;
    vector<vector<string>> point1(3, vector<string>(2)), point2(3, vector<string>(2));
    point1[0][0] = "3639014772220302969239478380372569697853575812585274288911649265451082883565179365094575093493765591320603987709986";
    point1[0][1] = "3627009162346736306815369238116921210498219699819586442010150505939503082090644552148957421881475528065715370270354";
    point1[1][0] = "581916622597845089745272661946997319695944788597773787799484754377746100203623348158129927080369032982420039191898";
    point1[1][1] = "2060995263553188122589792738829838608534517245971325324573983541174135610487090208942105316431342337272892000140574";
    point1[2][0] = "1379037705404560079908242963533160741216777276968477553205923477910255218220226843227833291221018680599293919690139";
    point1[2][1] = "1222016848140062171998821910705868456778091862872885965746947445153530098486346123336986882945598612334224853939670";

    point2[0][0] = "2057737302707558765860213632248591496828971052531394884004744357683286127161347650010073361956086815484639976049181";
    point2[0][1] = "2100089152630115104004378918944334063291538741442441275914861643724932134229574826973214215180219764947165540975080";
    point2[1][0] = "2761046348939817006370345598619575389565977388607130703090367184754327681186304224286762185646017408040537184404026";
    point2[1][1] = "3591610773756270231096263094409160844816105827629891713449617733426148420079417406316994217582407782801806318076355";
    point2[2][0] = "361245890999868516264908086411891038264258441593807793882550805776209472941545014795641271868722093733067921730065";
    point2[2][1] = "578345463844648614472854569382514076495684244079273140322468994353640333794696894886030050136893687175790979496041";

    Complex_Plain g1x, g1y, g1z, g2x, g2y, g2z;
    str_to_gfp(g1x.real, point1[0][0]);
    str_to_gfp(g1x.imag, point1[0][1]);
    str_to_gfp(g1y.real, point1[1][0]);
    str_to_gfp(g1y.imag, point1[1][1]);
    str_to_gfp(g1z.real, point1[2][0]);
    str_to_gfp(g1z.imag, point1[2][1]);

    str_to_gfp(g2x.real, point2[0][0]);
    str_to_gfp(g2x.imag, point2[0][1]);
    str_to_gfp(g2y.real, point2[1][0]);
    str_to_gfp(g2y.imag, point2[1][1]);
    str_to_gfp(g2z.real, point2[2][0]);
    str_to_gfp(g2z.imag, point2[2][1]);

    G2_Jacobian_Coordinates_Plain jcp(g2x, g2y, g2z);

    vector<G2_Jacobian_Coordinates> jc(2);
    get_inputs(0, jc[0].X, g1x);
    get_inputs(0, jc[0].Y, g1y);
    get_inputs(0, jc[0].Z, g1z);

    get_inputs(1, jc[1].X, g2x);
    get_inputs(1, jc[1].Y, g2y);
    get_inputs(1, jc[1].Z, g2z);
    cout << "--------test add_affine--------" << endl;
    G2_Jacobian_Coordinates res(jc[0]);
    reveal_and_print(res);
    cout << "*******\n";
    reveal_and_print(jc[1]);
    cout << "*******\n";
    add_jac_inplace(res, jc[1]);
    reveal_and_print(res);
    cout << "--------test add_plain_affine--------" << endl;

    res = jc[0];
    add_plain_jac_inplace(res, jcp);
    reveal_and_print(res);
}

void G2Op::G2_test_add_proj()
{
    cout << "============================== BEG " << __FUNCTION__ << " ==============================" << endl;
    vector<vector<string>> point1(3, vector<string>(2)), point2(3, vector<string>(2));
    point1[0][0] = "352701069587466618187139116011060144890029952792775240219908644239793785735715026873347600343865175952761926303160";
    point1[0][1] = "3059144344244213709971259814753781636986470325476647558659373206291635324768958432433509563104347017837885763365758";
    point1[1][0] = "1985150602287291935568054521177171638300868978215655730859378665066344726373823718423869104263333984641494340347905";
    point1[1][1] = "927553665492332455747201965776037880757740193453592970025027978793976877002675564980949289727957565575433344219582";
    point1[2][0] = "1";
    point1[2][0] = "0";

    point2[0][0] = "3615371968425162313504701611584580861091541671906640286537610648981546277559811355418121120405186865192603156540471";
    point2[0][1] = "1996123912207319781232871775273589916094509852833549867151637455520658731483328327777718782754201260420931019497944";
    point2[1][0] = "978142457653236052983988388396292566217089069272380812666116929298652861694202207333864830606577192738105844024927";
    point2[1][1] = "2248711152455689790114026331322133133284196260289964969465268080325775757898907753181154992709229860715480504777099";
    point2[2][0] = "3970301204574583871136109042354343276601737956431311461718757330132689452747647436847738208526667969282988680695810";
    point2[2][1] = "1855107330984664911494403931552075761515480386907185940050055957587953754005351129961898579455915131150866688439164";

    Complex_Plain g1x, g1y, g1z, g2x, g2y, g2z;
    str_to_gfp(g1x.real, point1[0][0]);
    str_to_gfp(g1x.imag, point1[0][1]);
    str_to_gfp(g1y.real, point1[1][0]);
    str_to_gfp(g1y.imag, point1[1][1]);
    str_to_gfp(g1z.real, point1[2][0]);
    str_to_gfp(g1z.imag, point1[2][1]);

    str_to_gfp(g2x.real, point2[0][0]);
    str_to_gfp(g2x.imag, point2[0][1]);
    str_to_gfp(g2y.real, point2[1][0]);
    str_to_gfp(g2y.imag, point2[1][1]);
    str_to_gfp(g2z.real, point2[2][0]);
    str_to_gfp(g2z.imag, point2[2][1]);

    G2_Projective_Coordinates_Plain pcp(g2x, g2y, g2z);

    vector<G2_Projective_Coordinates> pc(2);
    get_inputs(0, pc[0].X, g1x);
    get_inputs(0, pc[0].Y, g1y);
    get_inputs(0, pc[0].Z, g1z);

    get_inputs(1, pc[1].X, g2x);
    get_inputs(1, pc[1].Y, g2y);
    get_inputs(1, pc[1].Z, g2z);
    cout << "--------test add_affine--------" << endl;
    G2_Projective_Coordinates res(pc[0]);
    reveal_and_print(res);
    cout << "*******\n";
    reveal_and_print(pc[1]);
    cout << "*******\n";
    add_proj_inplace(res, pc[1]);
    reveal_and_print(res);
    cout << "--------test add_plain_affine--------" << endl;

    res = pc[0];
    add_plain_proj_inplace(res, pcp);
    reveal_and_print(res);
}