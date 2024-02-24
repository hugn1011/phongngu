//Chương trình vẽ 1 hình lập phương đơn vị theo mô hình lập trình OpenGL hiện đại

#include "Angel.h"  /* Angel.h là file tự phát triển (tác giả Prof. Angel), có chứa cả khai báo includes glew và freeglut*/
#include "math.h"
#include "ctime"

// remember to prototype
void generateGeometry(void);
void initGPUBuffers(void);
void shaderSetup(void);
void display(void);
void keyboard(unsigned char key, int x, int y);


typedef vec4 point4;
typedef vec4 color4;
using namespace std;

// Số các đỉnh của các tam giác
const int NumPoints = 36;

point4 points[NumPoints]; /* Danh sách các đỉnh của các tam giác cần vẽ*/
color4 colors[NumPoints]; /* Danh sách các màu tương ứng cho các đỉnh trên*/
vec3 normals[NumPoints]; /*Danh sách các vector pháp tuyến ứng với mỗi đỉnh*/


point4 vertices[8]; /* Danh sách 8 đỉnh của hình lập phương*/
color4 vertex_colors[8]; /*Danh sách các màu tương ứng cho 8 đỉnh hình lập phương*/

GLuint program;


GLint matrix_loc;

mat4 projection;
GLuint projection_loc;

mat4 view;
GLuint view_loc;
GLfloat mauAnhSang = 1.0;


void initCube()
{
	// Gán giá trị tọa độ vị trí cho các đỉnh của hình lập phương
	vertices[0] = point4(-0.5, -0.5, 0.5, 1.0);
	vertices[1] = point4(-0.5, 0.5, 0.5, 1.0);
	vertices[2] = point4(0.5, 0.5, 0.5, 1.0);
	vertices[3] = point4(0.5, -0.5, 0.5, 1.0);
	vertices[4] = point4(-0.5, -0.5, -0.5, 1.0);
	vertices[5] = point4(-0.5, 0.5, -0.5, 1.0);
	vertices[6] = point4(0.5, 0.5, -0.5, 1.0);
	vertices[7] = point4(0.5, -0.5, -0.5, 1.0);

	// Gán giá trị màu sắc cho các đỉnh của hình lập phương	
	vertex_colors[0] = color4(0.0, 0.0, 0.0, 1.0); // black
	vertex_colors[1] = color4(1.0, 0.0, 0.0, 1.0); // red
	vertex_colors[2] = color4(1.0, 1.0, 0.0, 1.0); // yellow
	vertex_colors[3] = color4(0.0, 1.0, 0.0, 1.0); // green
	vertex_colors[4] = color4(0.0, 0.0, 1.0, 1.0); // blue
	vertex_colors[5] = color4(1.0, 0.0, 1.0, 1.0); // magenta
	vertex_colors[6] = color4(0.0, 1.0, 1.0, 1.0); // cyan
	vertex_colors[7] = color4(1.0, 1.0, 1.0, 1.0); // white
}
int Index = 0;
void quad(int a, int b, int c, int d)
/*Tạo một mặt hình lập phương = 2 tam giác, gán màu cho mỗi đỉnh tương ứng trong mảng colors*/
{
	vec4 u = vertices[b] - vertices[a];
	vec4 v = vertices[c] - vertices[b];
	vec3 normal = normalize(cross(u, v));

	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[b]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[c]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[c]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[d]; Index++;

}
void makeColorCube(void)  /* Sinh ra 12 tam giác: 36 đỉnh, 36 màu*/

{
	quad(5, 4, 0, 1);
	quad(1, 0, 3, 2);
	quad(6, 5, 1, 2);
	quad(4, 5, 6, 7);
	quad(3, 0, 4, 7);
	quad(2, 3, 7, 6);
}
void generateGeometry(void)
{
	initCube();
	makeColorCube();
}


void initGPUBuffers(void)
{
	// Tạo một VAO - vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Tạo và khởi tạo một buffer object
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors) + sizeof(normals), NULL, GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors), sizeof(normals), normals);

}

void shaderSetup(void)
{
	// Nạp các shader và sử dụng chương trình shader
	program = InitShader("vshader1.glsl", "fshader1.glsl");   // hàm InitShader khai báo trong Angel.h
	glUseProgram(program);

	// Khởi tạo thuộc tính vị trí đỉnh từ vertex shader
	GLuint loc_vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(loc_vPosition);
	glVertexAttribPointer(loc_vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	GLuint loc_vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(loc_vColor);
	glVertexAttribPointer(loc_vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points)));

	GLuint loc_vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(loc_vNormal);
	glVertexAttribPointer(loc_vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points) + sizeof(colors)));



	matrix_loc = glGetUniformLocation(program, "model_view");
	projection_loc = glGetUniformLocation(program, "Projection");
	view_loc = glGetUniformLocation(program, "View");

	glEnable(GL_DEPTH_TEST);
	glClearColor(1.0, 1.0, 1.0, 1.0);        /* Thiết lập màu trắng là màu xóa màn hình*/

}
class matrix_stack /*Khai báo lớp stack ma trận*/
{
private:
	static const int MAX = 50;
	mat4 matrices[MAX]; /*Lưu các ma trận của stack*/
	int index; /*Chỉ số truy cập phần tử đầu tiên*/

public: /*Số phần tử tối đa của stack*/
	matrix_stack() { index = 0; } /*Constructor: Khởi gán chỉ số phần tử đầu tiên là 0*/
	void push(const mat4& matrix) {
		matrices[index] = matrix;
		index++;
	};
	mat4 pop() {
		index--;
		return matrices[index];
	};
};
mat4 ctm;
matrix_stack stack;

GLfloat theta[] = { 0,0,0,0 };
void paintColor(GLfloat a, GLfloat b, GLfloat c) {
	/* Khởi tạo các tham số chiếu sáng - tô bóng*/
	point4 light_position(3.17, 1.93, 0.0, 0.0);
	color4 light_ambient(0.2, 0.2, 0.2, 1.0);
	color4 light_diffuse(mauAnhSang, mauAnhSang, mauAnhSang, 1.0);
	color4 light_specular(0.0, 0.0, 0.0, 1.0);

	color4 material_ambient(1.0, 0.0, 1.0, 1.0);
	color4 material_diffuse(a / 255.0, b / 255.0, c / 255.0, 1.0);
	color4 material_specular(255.0 / 255.0, 255.0 / 255.0, 255 / 255.0, 1.0);
	float material_shininess = 100.0;

	color4 ambient_product = light_ambient * material_ambient;
	color4 diffuse_product = light_diffuse * material_diffuse;
	color4 specular_product = light_specular * material_specular;

	glUniform4fv(glGetUniformLocation(program, "AmbientProduct"), 1, ambient_product);
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);
	glUniform4fv(glGetUniformLocation(program, "SpecularProduct"), 1, specular_product);
	glUniform4fv(glGetUniformLocation(program, "LightPosition"), 1, light_position);
	glUniform1f(glGetUniformLocation(program, "Shininess"), material_shininess);
}

void veTuong() {
	paintColor(100, 255, 255);
	//Tường sau
	ctm = Translate(0, 0, 4)* Scale(8.0, 5.0, 0.05);
	glUniformMatrix4fv(matrix_loc, 1, GL_TRUE, ctm);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	//Tường trước
	ctm = Translate(1.0, 0, -4)* Scale(6.0, 5.0, 0.05);
	glUniformMatrix4fv(matrix_loc, 1, GL_TRUE, ctm);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	ctm = Translate(-3, 1.5, -4)* Scale(2.0, 2.0, 0.05);
	glUniformMatrix4fv(matrix_loc, 1, GL_TRUE, ctm);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);


	//Tường trái
	ctm = Translate(-4, 0, 0.0)* Scale(0.05, 5.0, 8.0);
	glUniformMatrix4fv(matrix_loc, 1, GL_TRUE, ctm);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	//Tường phải
	ctm = Translate(4, 0, 0.0)* Scale(0.05, 5.0, 8.0);
	glUniformMatrix4fv(matrix_loc, 1, GL_TRUE, ctm);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	//Đáy
	ctm = Translate(0.0, -2.5, 0.0)* Scale(8.0, 0.05, 8.0);
	glUniformMatrix4fv(matrix_loc, 1, GL_TRUE, ctm);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	//Trần 
	paintColor(236, 236, 236);
	ctm = Translate(0.0, 2.5, 0.0)* Scale(8.0, 0.05, 8.0);
	glUniformMatrix4fv(matrix_loc, 1, GL_TRUE, ctm);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

}
void veCuaRaVao() {
	// Cánh cửa
	paintColor(252.0, 245.0, 76.0);
	stack.push(ctm);
	ctm *= Translate(1.0, 0.0, 0.0)* Scale(2.0, 3.0, 0.05);
	glUniformMatrix4fv(matrix_loc, 1, GL_TRUE, ctm);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	ctm = stack.pop();
	//Núm cửa
	paintColor(0, 0, 0);
	stack.push(ctm);
	ctm *= Translate(1.7, 0.0, 0.075) * Scale(0.05, 0.3, 0.1);
	glUniformMatrix4fv(matrix_loc, 1, GL_TRUE, ctm);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	ctm = stack.pop();
	//Núm cửa
	paintColor(0, 0, 0);
	ctm *= Translate(1.7, 0.0, -0.075) * Scale(0.05, 0.3, 0.1);
	glUniformMatrix4fv(matrix_loc, 1, GL_TRUE, ctm);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
GLfloat rotateCuaRaVao;
void veTuongVaCuaRaVao() {
	//stack.push(ctm);
	veTuong();
	ctm = Translate(-4, -1.0, -4) * RotateY(rotateCuaRaVao);
	veCuaRaVao();
	//ctm = stack.pop();
}

GLfloat mauDen = 255;
void bongDen() {
	paintColor(mauDen, mauDen, mauDen);
	for (int i = 0; i <= 180; i++) {
		ctm = stack.pop();
		stack.push(ctm);
		ctm *= RotateZ(i) * Scale(0.06, 0.06, 2);
		glUniformMatrix4fv(matrix_loc, 1, GL_TRUE, ctm);
		glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	}
}

void beDen() {
	paintColor(255, 255, 255);
	ctm = stack.pop();
	stack.push(ctm);
	ctm *= Translate(0.08, 0, 0) * Scale(0.01, 0.1, 2.2);
	glUniformMatrix4fv(matrix_loc, 1, GL_TRUE, ctm);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	paintColor(0, 162, 232);
	ctm = stack.pop();
	stack.push(ctm);
	ctm *= Translate(0, 0, 1.05) * Scale(0.1, 0.1, 0.1);
	glUniformMatrix4fv(matrix_loc, 1, GL_TRUE, ctm);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	ctm = stack.pop();
	ctm *= Translate(0, 0, -1.05) * Scale(0.08, 0.1, 0.1);
	glUniformMatrix4fv(matrix_loc, 1, GL_TRUE, ctm);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void den() {
	ctm = Translate(3.2, 1.9, 0);
	stack.push(ctm);
	bongDen();
	beDen();
}

//vẽ tủ
void veThanTu()
{
	//Vẽ thanh ngang giữa tủ
	paintColor(252.0, 245.0, 76.0);
	stack.push(ctm);
	ctm *= Scale(0.02, 1.0, 0.5);
	glUniformMatrix4fv(matrix_loc, 1, GL_TRUE, ctm);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	ctm = stack.pop();

	//Vẽ thanh ngang bên trái
	paintColor(252.0, 245.0, 76.0);
	stack.push(ctm);
	ctm *= Translate(0.4, -0.15, 0.0)*Scale(0.02, 1.3, 0.5);
	glUniformMatrix4fv(matrix_loc, 1, GL_TRUE, ctm);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	ctm = stack.pop();

	//Vẽ thanh ngang bên phải tủ
	paintColor(252.0, 245.0, 76.0);
	stack.push(ctm);
	ctm *= Translate(-0.4, -0.15, 0.0)*Scale(0.02, 1.3, 0.5);
	glUniformMatrix4fv(matrix_loc, 1, GL_TRUE, ctm);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	ctm = stack.pop();

	//Vẽ mặt trên tủ
	paintColor(252.0, 245.0, 76.0);
	stack.push(ctm);
	ctm *= Translate(0.0, 0.51, 0.0)*Scale(0.82, 0.02, 0.5);
	glUniformMatrix4fv(matrix_loc, 1, GL_TRUE, ctm);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	ctm = stack.pop();

	//Vẽ mặt giữa tủ
	paintColor(252.0, 245.0, 76.0);
	stack.push(ctm);
	ctm *= Translate(0.0, -0.51, 0.0)*Scale(0.78, 0.02, 0.5);
	glUniformMatrix4fv(matrix_loc, 1, GL_TRUE, ctm);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	ctm = stack.pop();

	//Vẽ mặt đáy tủ
	paintColor(252.0, 245.0, 76.0);
	stack.push(ctm);
	ctm *= Translate(0.0, -0.79, 0.0)*Scale(0.78, 0.02, 0.5);
	glUniformMatrix4fv(matrix_loc, 1, GL_TRUE, ctm);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	ctm = stack.pop();

	//Vẽ mặt lưng tủ
	paintColor(252.0, 245.0, 76.0);
	stack.push(ctm);
	ctm *= Translate(0.0, -0.15, 0.26)*Scale(0.82, 1.32, 0.02);
	glUniformMatrix4fv(matrix_loc, 1, GL_TRUE, ctm);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	ctm = stack.pop();
}
void veCanhTraiTu() {
	//Vẽ cánh trái tủ
	paintColor(252.0, 245.0, 76.0);
	stack.push(ctm);
	ctm *= Scale(0.41, 1.04, 0.02);
	glUniformMatrix4fv(matrix_loc, 1, GL_TRUE, ctm);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	ctm = stack.pop();

	//Tay cầm tủ cánh trái
	paintColor(131.0, 199.0, 93.0);
	stack.push(ctm);
	ctm *= Translate(0.15, 0.0, -0.025) * Scale(0.03, 0.2, 0.03);
	glUniformMatrix4fv(matrix_loc, 1, GL_TRUE, ctm);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	ctm = stack.pop();
}
void veCanhPhaiTu() {
	//Vẽ cánh trái phải
	paintColor(252.0, 245.0, 76.0);
	stack.push(ctm);
	ctm *= Scale(0.41, 1.04, 0.02);
	glUniformMatrix4fv(matrix_loc, 1, GL_TRUE, ctm);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	ctm = stack.pop();

	//Tay cầm tủ cánh phải
	paintColor(131.0, 199.0, 93.0);
	stack.push(ctm);
	ctm *= Translate(-0.15, 0.0, -0.025)*Scale(0.03, 0.2, 0.03);
	glUniformMatrix4fv(matrix_loc, 1, GL_TRUE, ctm);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	ctm = stack.pop();
}
void veNganKeo() {
	//Vẽ mặt đáy ngăn kéo
	paintColor(252.0, 210.0, 76.0);
	stack.push(ctm);
	ctm *= Translate(0.0, -0.77, 0.0)*Scale(0.76, 0.02, 0.5);
	glUniformMatrix4fv(matrix_loc, 1, GL_TRUE, ctm);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	ctm = stack.pop();

	//Vẽ mặt trái ngăn kéo
	paintColor(252.0, 245.0, 76.0);
	stack.push(ctm);
	ctm *= Translate(-0.39, -0.65, 0.0)*Scale(0.02, 0.26, 0.5);
	glUniformMatrix4fv(matrix_loc, 1, GL_TRUE, ctm);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	ctm = stack.pop();

	//Vẽ mặt phải ngăn kéo
	paintColor(252.0, 245.0, 76.0);
	stack.push(ctm);
	ctm *= Translate(0.39, -0.65, 0.0)*Scale(0.02, 0.26, 0.5);
	glUniformMatrix4fv(matrix_loc, 1, GL_TRUE, ctm);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	ctm = stack.pop();

	//Vẽ mặt trước
	paintColor(252.0, 245.0, 76.0);
	stack.push(ctm);
	ctm *= Translate(0.0, -0.66, -0.26)*Scale(0.82, 0.28, 0.02);
	glUniformMatrix4fv(matrix_loc, 1, GL_TRUE, ctm);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	ctm = stack.pop();

	//Tay cầm ngăn kéo
	paintColor(131.0, 199.0, 93.0);
	stack.push(ctm);
	ctm *= Translate(0.0, -0.66, -0.285)*Scale(0.2, 0.03, 0.03);
	glUniformMatrix4fv(matrix_loc, 1, GL_TRUE, ctm);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	ctm = stack.pop();
}
GLfloat rotateCanhTu[] = { 0,0 };
GLfloat translateNganKeo = 0;
GLfloat translateTuX, translateTuZ, rotateTuY;
void veTu() {
	mat4 tmp;
	ctm *= Translate(translateTuX, 0.0, translateTuZ)*RotateY(rotateTuY);
	tmp = ctm;
	veThanTu();

	ctm *= Translate(-0.41, 0.0, -0.26) * RotateY(rotateCanhTu[0])*Translate(0.205, 0.0, 0.0);
	veCanhTraiTu();

	ctm = tmp;
	ctm *= Translate(0.41, 0.0, -0.26) * RotateY(rotateCanhTu[1])*Translate(-0.205, 0.0, 0.0);
	veCanhPhaiTu();

	ctm = tmp;
	ctm *= Translate(0.0, 0.0, translateNganKeo);
	veNganKeo();
}

GLfloat kimGio = (((time(0) / 60 / 60) - 5) % 24) * 30;
GLfloat kimPhut = ((time(0) / 60) % 60) * 6;
GLfloat kimGiay = (time(0) % 60) * 6;
void veDongHo() {
	//Vẽ mặt đồng hồ
	paintColor(255, 255, 255);
	stack.push(ctm);
	for (int i = 0; i <= 90; i++) {
		ctm = stack.pop();
		stack.push(ctm);
		ctm *= Translate(0.0, 0.11, 0.0) * RotateZ(i) * Scale(0.44, 0.44, 0.02);
		glUniformMatrix4fv(matrix_loc, 1, GL_TRUE, ctm);
		glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	}
	ctm = stack.pop();
	//vẽ kim giây
	paintColor(255.0, 0.0, 0.0);
	stack.push(ctm);
	ctm *= Translate(0, 0.11, -0.0125) * RotateZ(kimGiay) * Translate(0, 0.1, 0) * Scale(0.01, 0.2, 0.01);
	glUniformMatrix4fv(matrix_loc, 1, GL_TRUE, ctm);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	ctm = stack.pop();
	//Vẽ kim phút
	paintColor(0.0, 0.0, 0.0);
	stack.push(ctm);
	ctm *= Translate(0, 0.11, -0.0125) * RotateZ(kimPhut) * Translate(0, 0.1, 0) * Scale(0.02, 0.2, 0.01);
	glUniformMatrix4fv(matrix_loc, 1, GL_TRUE, ctm);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	ctm = stack.pop();
	//Vẽ kim giờ
	paintColor(0.0, 0.0, 0.0);
	stack.push(ctm);
	ctm *= Translate(0.0, 0.11, -0.0125) * RotateZ(kimGio) * Translate(0, 0.08, 0) * Scale(0.02, 0.16, 0.01);
	glUniformMatrix4fv(matrix_loc, 1, GL_TRUE, ctm);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	ctm = stack.pop();

}

void timer(int t) {
	glutTimerFunc(1000, timer, 0);
	if (kimGiay == 360) {
		kimGiay = 0;
		kimPhut += 6;
		if (kimPhut == 360) {
			kimGio += 30;
			kimPhut = 0;
		}
	}
	kimGiay += 6;
	glutPostRedisplay();
}

//Bàn
GLfloat translateBanX = 0, translateBanZ = 0, translateNganBanX = 0;
void matBan() {
	ctm *= Scale(1.3, 0.1, 2);
	glUniformMatrix4fv(matrix_loc, 1, GL_TRUE, ctm);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

//chân bàn
void chanBan() {
	ctm = stack.pop();
	stack.push(ctm);
	ctm *= Translate(0.6, -0.65, -0.95) * Scale(0.1, 1.2, 0.1);
	glUniformMatrix4fv(matrix_loc, 1, GL_TRUE, ctm);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	ctm = stack.pop();
	stack.push(ctm);
	ctm *= Translate(0.6, -0.65, 0.95) * Scale(0.1, 1.2, 0.1);
	glUniformMatrix4fv(matrix_loc, 1, GL_TRUE, ctm);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	ctm = stack.pop();
	stack.push(ctm);
	ctm *= Translate(-0.6, -0.65, -0.95) * Scale(0.1, 1.2, 0.1);
	glUniformMatrix4fv(matrix_loc, 1, GL_TRUE, ctm);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	ctm = stack.pop();
	stack.push(ctm);
	ctm *= Translate(-0.6, -0.65, 0.95) * Scale(0.1, 1.2, 0.1);
	glUniformMatrix4fv(matrix_loc, 1, GL_TRUE, ctm);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void nganKeoBan() {
	ctm = stack.pop();
	stack.push(ctm);
	ctm *= Translate(translateNganBanX, 0, 0) * Translate(0, -0.15, 0) * Scale(1.3, 0.2, 1.5);
	glUniformMatrix4fv(matrix_loc, 1, GL_TRUE, ctm);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	ctm = stack.pop();
}

void ban() {
	paintColor(252.0, 245.0, 76.0);
	ctm = Translate(translateBanX, 0, translateBanZ) * Translate(-3.3, -1.2, 0);
	stack.push(ctm);
	matBan();
	chanBan();
	nganKeoBan();
}

//ghế
GLfloat rotateGheY = 0, rotateGheZ = 0;
GLfloat translateGheX = 0, translateGheZ = 0;
void matGhe() {
	ctm = stack.pop();
	stack.push(ctm);
	ctm *= Scale(0.8, 0.05, 1);
	glUniformMatrix4fv(matrix_loc, 1, GL_TRUE, ctm);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void chanGhe(GLfloat x, GLfloat y, GLfloat z) {
	ctm = stack.pop();
	stack.push(ctm);
	ctm *= Translate(x, y, z) * Scale(0.08, 0.8, 0.08);
	glUniformMatrix4fv(matrix_loc, 1, GL_TRUE, ctm);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void tuaGhe() {
	ctm = stack.pop();
	ctm *= Translate(-0.375, 0.025, 0) * RotateZ(rotateGheZ) * Translate(0, 0.5, 0) * Scale(0.05, 1, 1);
	glUniformMatrix4fv(matrix_loc, 1, GL_TRUE, ctm);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void ghe() {
	paintColor(82.0, 78.0, 250.0);
	ctm = Translate(translateGheX, 0, translateGheZ) * Translate(-2, -1.525, 0) * RotateY(rotateGheY) * RotateY(180);
	stack.push(ctm);
	matGhe();
	chanGhe(0.36, -0.425, 0.46);
	chanGhe(0.36, -0.425, -0.46);
	chanGhe(-0.36, -0.425, -0.46);
	chanGhe(-0.36, -0.425, 0.46);
	tuaGhe();
}

//giường
void matGiuong() {
	ctm = stack.pop();
	stack.push(ctm);
	ctm *= Scale(3, 0.08, 4);
	glUniformMatrix4fv(matrix_loc, 1, GL_TRUE, ctm);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void chanGiuong(GLfloat x, GLfloat y, GLfloat z) {
	ctm = stack.pop();
	stack.push(ctm);
	ctm *= Translate(x, y, z) * Scale(0.2, 0.5, 0.2);
	glUniformMatrix4fv(matrix_loc, 1, GL_TRUE, ctm);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void thanhGiuong() {
	ctm = stack.pop();
	stack.push(ctm);
	ctm *= Translate(0, 0.24, 1.9) * Scale(3, 0.4, 0.2);
	glUniformMatrix4fv(matrix_loc, 1, GL_TRUE, ctm);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	ctm = stack.pop();
	ctm *= Translate(0, 0.14, -1.9) * Scale(3, 0.2, 0.2);
	glUniformMatrix4fv(matrix_loc, 1, GL_TRUE, ctm);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void giuong() {
	paintColor(252.0, 245.0, 76.0);
	ctm = Translate(2.5, -1.96, 2);
	stack.push(ctm);
	matGiuong();
	chanGiuong(1.4, -0.29, 1.9);
	chanGiuong(1.4, -0.29, -1.9);
	chanGiuong(-1.4, -0.29, -1.9);
	chanGiuong(-1.4, -0.29, 1.9);
	thanhGiuong();
}

//Quạt
void mocTreo() {
	ctm *= Translate(0, 0.31, 0) * Scale(0.1, 0.5, 0.1);
	glUniformMatrix4fv(matrix_loc, 1, GL_TRUE, ctm);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}

GLfloat gocQuay = 0;
void cuQuat() {
	for (int i = 0; i <= 90; i++) {
		ctm = stack.pop();
		stack.push(ctm);
		ctm *= RotateY(i) * Scale(0.35, 0.12, 0.35);
		glUniformMatrix4fv(matrix_loc, 1, GL_TRUE, ctm);
		glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
	}
}

void canhQuat(GLfloat y) {
	for (int i = 0; i <= 90; i++) {
		//RotateY(gocQuay) * Translate(1.5, 1.65, 0) * Translate(0.75, 1.65, 0) *
		ctm = stack.pop();
		stack.push(ctm);
		ctm *= RotateY(y) * Translate(1.4, 0, 0) * RotateY(i) * Scale(0.35, 0.05, 0.35);
		glUniformMatrix4fv(matrix_loc, 1, GL_TRUE, ctm);
		glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	}
	ctm = stack.pop();
	stack.push(ctm);
	ctm *= RotateY(y) * Translate(0.65, 0, 0) *  Scale(1.3, 0.05, 0.5);
	glUniformMatrix4fv(matrix_loc, 1, GL_TRUE, ctm);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

}

void quat() {
	paintColor(6, 105, 225);
	ctm = Translate(0, 1.89, 0);
	stack.push(ctm);
	mocTreo();
	ctm = stack.pop();
	ctm *= RotateY(gocQuay);
	stack.push(ctm);
	cuQuat();
	canhQuat(10);
	canhQuat(130);
	canhQuat(250);
	ctm = stack.pop();
}

//điều hòa

void model_dieuHoa(GLfloat scaleZ, GLfloat translateZ, GLint kt) {
	ctm = Translate(-3.8, 2, 0) * Translate(0, -0.0025, translateZ) * Scale(0.3, 0.005, scaleZ);
	glUniformMatrix4fv(matrix_loc, 1, GL_TRUE, ctm);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	for (int i = 2; i <= 30; i++) {
		ctm = Translate(-3.8, 2, 0) * Translate(0.0005 * i, -0.005 * i, translateZ) * Scale(0.3 + 0.001 * i, 0.005, scaleZ);
		glUniformMatrix4fv(matrix_loc, 1, GL_TRUE, ctm);
		glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	}
	int j = 30;
	for (int i = 30; i <= 50; i++) {
		ctm = Translate(-3.8, 2, 0) * Translate(0.0005 * j, -0.005 * i, translateZ) * Scale(0.3 + 0.001 * j, 0.005, scaleZ);
		glUniformMatrix4fv(matrix_loc, 1, GL_TRUE, ctm);
		glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	}
	j = 50;
	for (int i = 30; i >= kt; i--) {
		j++;
		ctm = Translate(-3.8, 2, 0) * Translate(0.0005 * i, -0.005 * j, translateZ) * Scale(0.3 + 0.001 * i, 0.005, scaleZ);
		glUniformMatrix4fv(matrix_loc, 1, GL_TRUE, ctm);
		glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	}
}

float swing_dieuHoa = 75.0;
void dieuHoa() {
	paintColor(192, 192, 192);
	model_dieuHoa(0.7, 0, 25);
	model_dieuHoa(0.1, 0.4, 1);
	model_dieuHoa(0.1, -0.4, 1);
	//cánh mở điều hòa
	ctm = Translate(-3.8, 2, 0) * Translate(0.151, -0.385, 0) * RotateZ(swing_dieuHoa) * Translate(0.06, 0, 0) * Scale(0.12, 0.005, 0.7);
	glUniformMatrix4fv(matrix_loc, 1, GL_TRUE, ctm);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	ctm = Translate(-3.8, 2, 0) * Translate(0.0005, -0.3975, 0) * Scale(0.301, 0.05, 0.9);
	glUniformMatrix4fv(matrix_loc, 1, GL_TRUE, ctm);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

float swingRotation = 0.0;
bool swing = true;
void swingDieuHoa() {
	if (swing_dieuHoa + swingRotation >= -10 && swing_dieuHoa + swingRotation <= 75) {
		swing_dieuHoa += swingRotation;
	}
	glutPostRedisplay();
}

GLfloat doLech = 0;
GLfloat y = 0;
void spin() {
	if (y + 0.001 <= doLech) {
		gocQuay += y;
		y += 0.001;
		if (gocQuay + y >= 360)
			gocQuay -= 360;
	}
	if (y - 0.001 >= doLech) {
		gocQuay += y;
		y -= 0.001;
		if (gocQuay + y >= 360)
			gocQuay -= 360;
	}
	gocQuay += doLech;
	if (gocQuay + doLech >= 360)
		gocQuay -= 360;
	glutPostRedisplay();
}


GLfloat eyeX = 0.0, eyeZ = -4.0;
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	den();

	quat();

	veTuongVaCuaRaVao();

	ban();

	giuong();

	ghe();

	ctm = Translate(-2.975, -0.5, 3.325) * Scale(2.5, 2.5, 2.5);
	veTu();

	ctm = Translate(0, 1, 3.94) *  Scale(2.0, 2.0, 2.0);
	veDongHo();

	dieuHoa();

	vec4 eye(eyeX, 0, eyeZ, 1);
	vec4 at(0, 0, 0, 1);
	vec4 up(0, 1, 0, 1);
	view = LookAt(eye, at, up);
	glUniformMatrix4fv(view_loc, 1, GL_TRUE, view);

	projection = Frustum(-4, 4, -2, 2, 2, 10);
	glUniformMatrix4fv(projection_loc, 1, GL_TRUE, projection);

	glutSwapBuffers();
}
void resharp(GLint w, GLint h)
{
	glViewport(0, 0, w, h);
}

GLfloat round1(GLfloat x) {
	return roundf(x * 10) / 10;
}
bool batden = true;
void keyboard(unsigned char key, int x, int y)
{
	// keyboard handler

	switch (key) {
	case 033:			// 033 is Escape key octal value
		exit(1);		// quit program
		break;
	case 'o':
		if (batden) {
			
			mauAnhSang = 0.4;
		}
		else {
		
			mauAnhSang = 0.1;
		}
		batden = !batden;
		glutPostRedisplay();
		break;
	//dịch bàn X
	case 'a':
		if (translateBanX - 0.1 >= 0) {
			translateBanX -= 0.1;
		}
		glutPostRedisplay();
		break;
	case 'A':
		if (translateBanX + 0.1 <= 3.7) {
			translateBanX += 0.1;
		}
		glutPostRedisplay();
		break;
	//dịch bàn Z
	case 'q':
		if (translateBanZ - 0.1 >= -1) {
			translateBanZ -= 0.1;
		}
		glutPostRedisplay();
		break;
	case 'Q':
		if (translateBanZ + 0.1 <= 1) {
			translateBanZ += 0.1;
		}
		glutPostRedisplay();
		break;
	//kéo ngăn kéo bàn
	case 'E':
		if (translateNganBanX - 0.1 >= 0)
			translateNganBanX -= 0.1;
		glutPostRedisplay();
		break;
	case 'e':
		if (translateNganBanX + 0.1 <= 1.2)
			translateNganBanX += 0.1;
		glutPostRedisplay();
		break;
	//Translate ghế theo X
	case 'm':
		if (translateGheX + 0.1 <= 5)
			translateGheX += 0.1;
		glutPostRedisplay();
		break;
	case 'M':
		if (translateGheX - 0.1 >= -2.45)
			translateGheX -= 0.1;
		glutPostRedisplay();
		break;
	// Translate ghe theo Z
	case 'l':
		if (translateGheZ + 0.1 <= 2)
			translateGheZ += 0.1;
		glutPostRedisplay();
		break;
	case 'L':
		if (translateGheZ - 0.1 >= -2)
			translateGheZ -= 0.1;
		glutPostRedisplay();
		break;
	//Xoay ghế
	case 'u':
		rotateGheY += 5;
		glutPostRedisplay();
		break;
	case 'U':
		rotateGheY -= 5;
		glutPostRedisplay();
		break;
	//Gập ghế
	case 'h':
		if (rotateGheZ - 5 >= -90)
			rotateGheZ -= 5;
		glutPostRedisplay();
		break;
	case 'H':
		if (rotateGheZ + 5 <= 0)
			rotateGheZ += 5;
		glutPostRedisplay();
		break;
	// bật quạt
	case '1':
		glutIdleFunc(spin);
		doLech = 4;
		break;
	case '2':
		glutIdleFunc(spin);
		doLech = 14;
		break;
	case '3':
		glutIdleFunc(spin);
		doLech = 21;
		break;
	case '0':
		glutIdleFunc(spin);
		doLech = 0;
		break;
	case 'z': 
		if (translateTuX - 0.02 >= 0)
			translateTuX -= 0.02;
		glutPostRedisplay();
		break;
	case 'Z': 
		if (translateTuX + 0.02 <= 1)
			translateTuX += 0.02;
		glutPostRedisplay();
		break;
	case 'x': 
		if (translateTuZ - 0.02 >= -1.0) 
			translateTuZ -= 0.02;
		glutPostRedisplay();
		break;
	case 'X': 
		if (translateTuZ + 0.02 <= 0) 
			translateTuZ += 0.02;
		glutPostRedisplay();
		break;
	case 'c':
		if (translateTuZ <= -0.2) {
			// Tủ đứng sát tường thì không được phép quay
			rotateTuY += 5;
			if (rotateTuY + 5 >= 360) rotateTuY -= 360;
			glutPostRedisplay();
		}
		break;
	case 'C':
		if (translateTuZ <= -0.2) {
			rotateTuY -= 5;
			if (rotateTuY - 5 <= 360) rotateTuY += 360;
			glutPostRedisplay();
		}
		break;
	//Mở cửa tủ phải
	case 'v': 
		if (rotateCanhTu[0] + 5 <= 180) 
			rotateCanhTu[0] += 5;
		glutPostRedisplay();
		break;
	//Đóng cửa tủ phải
	case 'V': 
		if (rotateCanhTu[0] - 5 >= 0)
			rotateCanhTu[0] -= 5;
		glutPostRedisplay();
		break;
	// mở cửa tủ trái
	case 'b': 
		if (rotateCanhTu[1] - 5 >= -180) 
			rotateCanhTu[1] -= 5;
		glutPostRedisplay();
		break;
	// đóng cửa tủ trái
	case 'B': 
		if (rotateCanhTu[1] + 5 >= 0)
			rotateCanhTu[1] += 5;
		glutPostRedisplay();
		break;
	case 'n': 
		translateNganKeo -= 0.02;
		if (translateNganKeo - 0.02 < -0.4) 
			translateNganKeo = -0.4;
		glutPostRedisplay();
		break;
	case 'N': 
		translateNganKeo += 0.02;
		if (translateNganKeo + 0.02 > 0) 
			translateNganKeo = 0;
		glutPostRedisplay();
		break;
	//Cửa ra vào
	case 'Y': 
		rotateCuaRaVao += 5;
		if (rotateCuaRaVao + 5 >= 0) rotateCuaRaVao = 0;
		glutPostRedisplay();
		break;
	case 'y': 
		rotateCuaRaVao -= 5;
		if (rotateCuaRaVao - 5 <= -80) rotateCuaRaVao = -80;
		glutPostRedisplay();
		break;
	//điều hòa
	case 'p':
		glutIdleFunc(swingDieuHoa);
		swingRotation = -0.05;
		break;
	case 'P':
		glutIdleFunc(swingDieuHoa);
		swingRotation = 0.05;
		break;

		//Thay đổi góc nhìn quanh trục Y
	case '5':
		eyeX = round1(eyeX);
		eyeZ = round1(eyeZ);
		if (eyeX <= 0 && eyeX > -4 && round1(eyeZ) >= -4 && eyeZ < 0) {
			eyeX -= 0.1;
			eyeZ += 0.1;
		}
		else if (eyeX >= -4 && eyeX < 0 && eyeZ >= 0 && eyeZ < 4) {
			eyeX += 0.1;
			eyeZ += 0.1;
		}
		else if (eyeX >= 0 && eyeX < 4 && eyeZ <= 4 && eyeZ > 0) {
			eyeX += 0.1;
			eyeZ -= 0.1;
		}
		else if (eyeX <= 4 && eyeX > 0 && eyeZ <= 0 && eyeZ > -4) {
			eyeX -= 0.1;
			eyeZ -= 0.1;
		}
		glutPostRedisplay();
		break;
	//Thay đổi góc nhìn quanh trục Y ngược chiều kim đồng hồ
	case '4':
		eyeX = round1(eyeX);
		eyeZ = round1(eyeZ);
		if (eyeX >= 0 && eyeX < 4 && round1(eyeZ) >= -4 && eyeZ < 0) {
			eyeX += 0.1;
			eyeZ += 0.1;
		}
		else if (eyeX <= 4 && eyeX > 0 && eyeZ >= 0 && eyeZ < 4) {
			eyeX -= 0.1;
			eyeZ += 0.1;
		}
		else if (eyeX <= 0 && eyeX > -4 && eyeZ <= 4 && eyeZ > 0) {
			eyeX -= 0.1;
			eyeZ -= 0.1;
		}
		else if (eyeX >= -4 && eyeX < 0 && eyeZ <= 0 && eyeZ > -4) {
			eyeX += 0.1;
			eyeZ -= 0.1;
		}
		glutPostRedisplay();
		break;

	}
}


int main(int argc, char **argv)
{
	// main function: program starts here

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(800, 800);
	glutInitWindowPosition(100, 0);
	glutCreateWindow("Drawing a Cube");


	glewInit();

	generateGeometry();
	initGPUBuffers();
	shaderSetup();

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(resharp);
	glutTimerFunc(1000, timer, 0);
	glutMainLoop();
	return 0;
}
