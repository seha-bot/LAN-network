typedef float vec2[2];
void tm_vec2_copy(vec2 in, vec2 out)
{
    out[0] = in[0];
    out[1] = in[1];
}

float scrollWheel;
vec2 mousePos;
vec2 mouseDelta;
vec2 worldMouseDelta;
vec2 worldMousePosition;
int* keys = 0;
int* mouseKeys = 0;
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    scrollWheel = yoffset;
}
void cursor_position_callback(GLFWwindow* window, double x, double y) {
    mouseDelta[0] = x - mousePos[0];
    mouseDelta[1] = y - mousePos[1];
    mousePos[0] = x;
    mousePos[1] = y;
}
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if(action == GLFW_PRESS) nec_push(keys, key);
}
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if(action == GLFW_PRESS) nec_push(mouseKeys, button);
}
// vec2 worldMousePos() {
//     return worldMousePosition;
// }
char getKey(int key) {
    return glfwGetKey(window, key) == GLFW_PRESS;
}
char getKeyDown(int key) {
    for(int i = 0; i < nec_size(keys); i++) if(keys[i] == key) return 1;
    return 0;
}
char getMouseButton(int button) {
    return glfwGetMouseButton(window, button) == GLFW_PRESS;
}
char getMouseButtonDown(int button) {
    for(int i = 0; i < nec_size(mouseKeys); i++) if(mouseKeys[i] == button) return 1;
    return 0;
}
void init_input() {
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
}
void input_start() {
    // vec2 wMP = { ((mousePos[0] * 2) - manager->screenWidth) / (8 * manager->camSize), -((mousePos[1] * 2) - manager->screenHeight) / (8 * manager->camSize) };
    // worldMouseDelta = wMP - worldMousePosition;
    // worldMousePosition = wMP;
}
void input_end() {
    scrollWheel = 0;
    tm_vec2_copy((vec2){ 0.0f, 0.0f }, worldMouseDelta);
    tm_vec2_copy((vec2){ 0.0f, 0.0f }, mouseDelta);
    if(keys) nec_free(keys);
    if(mouseKeys) nec_free(mouseKeys);
}