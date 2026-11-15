// iGameGod Non-Jailbreak Injector - FIXED with actual paths
// Framework: w1z6p5en.framework, Binary: w1z6p5en
// COMPILE: xcrun -sdk iphoneos clang -shared -fobjc-arc -framework Foundation -framework UIKit -arch arm64 -miphoneos-version-min=13.0 -o iGameGodInjector.dylib iGameGodInjector_FIXED.c

#import <dlfcn.h>
#import <objc/runtime.h>
#import <objc/message.h>
#import <stdio.h>
#import <pthread.h>
#import <unistd.h>

static void* framework_handle = NULL;
static Class GameGodClass = NULL;

static void initialize_igg_framework() {
    // Wait briefly for app to fully launch
    usleep(500000); // 0.5 seconds
    
    // Actual framework path from build
    const char* framework_path = "@executable_path/Frameworks/w1z6p5en.framework/w1z6p5en";
    
    fprintf(stderr, "[iGameGod Injector] Loading framework: %s\n", framework_path);
    
    // Load the framework
    framework_handle = dlopen(framework_path, RTLD_LAZY | RTLD_GLOBAL);
    if (!framework_handle) {
        fprintf(stderr, "[iGameGod Injector] dlopen failed: %s\n", dlerror());
        // Try alternative paths
        const char* alt_paths[] = {
            "@executable_path/Frameworks/w1z6p5en.framework/w1z6p5en",
            "/Frameworks/w1z6p5en.framework/w1z6p5en",
            NULL
        };
        for (int i = 0; alt_paths[i]; i++) {
            framework_handle = dlopen(alt_paths[i], RTLD_LAZY | RTLD_GLOBAL);
            if (framework_handle) {
                fprintf(stderr, "[iGameGod Injector] Loaded via alt path: %s\n", alt_paths[i]);
                break;
            }
        }
        if (!framework_handle) {
            fprintf(stderr, "[iGameGod Injector] All paths failed\n");
            return;
        }
    }
    
    // Get the GameGod class
    GameGodClass = (Class)dlsym(framework_handle, "OBJC_CLASS_$_GameGod");
    if (!GameGodClass) {
        GameGodClass = objc_getClass("GameGod");
    }
    if (!GameGodClass) {
        fprintf(stderr, "[iGameGod Injector] GameGod class not found\n");
        return;
    }
    fprintf(stderr, "[iGameGod Injector] Found GameGod class: %p\n", GameGodClass);
    
    // Call prepareResourceBundle
    void (*msgSend)(Class, SEL) = (void*)objc_msgSend;
    msgSend(GameGodClass, sel_registerName("prepareResourceBundle"));
    fprintf(stderr, "[iGameGod Injector] prepareResourceBundle called\n");
    
    // Call prepareTemporaryStorage
    msgSend(GameGodClass, sel_registerName("prepareTemporaryStorage"));
    fprintf(stderr, "[iGameGod Injector] prepareTemporaryStorage called\n");
    
    // Call embedGameGod - adds draggable button
    msgSend(GameGodClass, sel_registerName("embedGameGod"));
    fprintf(stderr, "[iGameGod Injector] embedGameGod called - cheat menu ready!\n");
}

__attribute__((constructor))
static void igg_injector_init() {
    // Run on background thread to not block launch
    pthread_t thread;
    pthread_create(&thread, NULL, (void* (*)(void*))initialize_igg_framework, NULL);
    pthread_detach(thread);
}

__attribute__((visibility("default")))
void iGameGodInjectorShowMenu() {
    if (GameGodClass) {
        void (*msgSend)(Class, SEL) = (void*)objc_msgSend;
        msgSend(GameGodClass, sel_registerName("showGameGod"));
        fprintf(stderr, "[iGameGod Injector] showGameGod called\n");
    }
}