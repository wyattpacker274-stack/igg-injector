// iGameGod Non-Jailbreak Injector
// Loads iGameGod framework and shows cheat menu via DYLD_INSERT_LIBRARIES
// Does NOT use MobileSubstrate - pure dyld + Objective-C runtime
// COMPILE ON MAC: clang -shared -fobjc-arc -framework Foundation -framework UIKit -o iGameGodInjector.dylib iGameGodInjector.c

#import <dlfcn.h>
#import <objc/runtime.h>
#import <objc/message.h>
#import <stdio.h>
#import <pthread.h>

static pthread_once_t init_once = PTHREAD_ONCE_INIT;
static void* framework_handle = NULL;
static Class GameGodClass = NULL;

static void initialize_igg_framework() {
    // Framework path: @executable_path/Frameworks/{FRAMEWORK_NAME}.framework/{FW_BINARY_NAME}
    // These will be replaced at build time
    const char* framework_path = "@executable_path/Frameworks/{FRAMEWORK_NAME}.framework/{FW_BINARY_NAME}";
    
    // Load the framework
    framework_handle = dlopen(framework_path, RTLD_LAZY | RTLD_GLOBAL);
    if (!framework_handle) {
        fprintf(stderr, "[iGameGod Injector] Failed to load framework: %s\n", dlerror());
        return;
    }
    fprintf(stderr, "[iGameGod Injector] Loaded framework: %s\n", framework_path);
    
    // Get the GameGod class
    GameGodClass = (Class)dlsym(framework_handle, "OBJC_CLASS_$_GameGod");
    if (!GameGodClass) {
        // Try alternative symbol names
        GameGodClass = objc_getClass("GameGod");
    }
    if (!GameGodClass) {
        fprintf(stderr, "[iGameGod Injector] Failed to find GameGod class\n");
        return;
    }
    fprintf(stderr, "[iGameGod Injector] Found GameGod class: %p\n", GameGodClass);
    
    // Call prepareResourceBundle (caches bundle before dyld hides it)
    void (*prepareResourceBundle)(Class, SEL) = (void*)objc_msgSend;
    prepareResourceBundle(GameGodClass, sel_registerName("prepareResourceBundle"));
    fprintf(stderr, "[iGameGod Injector] Called prepareResourceBundle\n");
    
    // Call prepareTemporaryStorage
    prepareResourceBundle(GameGodClass, sel_registerName("prepareTemporaryStorage"));
    fprintf(stderr, "[iGameGod Injector] Called prepareTemporaryStorage\n");
    
    // Call embedGameGod - this embeds the window with draggable button
    prepareResourceBundle(GameGodClass, sel_registerName("embedGameGod"));
    fprintf(stderr, "[iGameGod Injector] Called embedGameGod - cheat menu ready\n");
}

__attribute__((constructor))
static void igg_injector_init() {
    // Initialize on a background thread to not block app launch
    pthread_t thread;
    pthread_create(&thread, NULL, (void* (*)(void*))initialize_igg_framework, NULL);
    pthread_detach(thread);
}

// Also provide a function that can be called manually if needed
__attribute__((visibility("default")))
void iGameGodInjectorShowMenu() {
    if (GameGodClass) {
        void (*showGameGod)(Class, SEL) = (void*)objc_msgSend;
        showGameGod(GameGodClass, sel_registerName("showGameGod"));
        fprintf(stderr, "[iGameGod Injector] Called showGameGod\n");
    }
}