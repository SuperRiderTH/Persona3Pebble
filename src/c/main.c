#include <pebble.h>

static Window *windowMain;
static TextLayer *layerTime, *layerDate, *layerWeekday;
//static Layer *layerBat;

static GFont fontTime, fontDate, fontWeekday;
static BitmapLayer *layerBack, *layerDarkWord, *layerWord, *layerMoon, *layerP3;


static GBitmap *sprBack, *sprDarkWord, *sprWord, *sprMoon, *sprP3;

#if defined(PBL_COLOR)
    static GBitmap  *sprGrey;
    static BitmapLayer *layerGrey; 
#endif

#if defined(PBL_ROUND)
    //Pretty hacky, but I could have done worse.
    static BitmapLayer *layerBackC;
#endif

//static GPoint posCircle = GPoint(110,12);

static char tmpWeekday[] = "A";
static char tmpTime[] = "AA";
static char varTime[] = "AA";



static void bluetooth_callback(bool connected) {
    // Show the Dark Hour background if bluetooth is disconnected.
    if (connected){
        gbitmap_destroy(sprBack);
        gbitmap_destroy(sprDarkWord);
        #if defined(PBL_COLOR)
            layer_set_hidden(bitmap_layer_get_layer(layerDarkWord), true);
            sprBack = gbitmap_create_with_resource(RESOURCE_ID_TOP_BLUE);
        #else
            sprBack = gbitmap_create_with_resource(RESOURCE_ID_HUD_BACK_BW);
        #endif
            bitmap_layer_set_bitmap(layerBack, sprBack);
        #if defined(PBL_ROUND)
            bitmap_layer_set_bitmap(layerBackC, sprBack);
        #endif
        
        layer_set_hidden(bitmap_layer_get_layer(layerWord), false);
        
        
    } else {
        gbitmap_destroy(sprBack);
        
        #if defined(PBL_COLOR)
            gbitmap_destroy(sprDarkWord);
            sprDarkWord = gbitmap_create_with_resource(RESOURCE_ID_DARK_WORD);
            bitmap_layer_set_bitmap(layerDarkWord, sprDarkWord);
            layer_set_hidden(bitmap_layer_get_layer(layerDarkWord), false);
            sprBack = gbitmap_create_with_resource(RESOURCE_ID_TOP_DARK);
        #else
            sprBack = gbitmap_create_with_resource(RESOURCE_ID_HUD_DARK_BW);
        #endif
            bitmap_layer_set_bitmap(layerBack, sprBack);
        #if defined(PBL_ROUND)
            bitmap_layer_set_bitmap(layerBackC, sprBack);
        #endif
        
        layer_set_hidden(bitmap_layer_get_layer(layerWord), true);
    }

    // Vibrate on disconnect.
    if( !connected ) {
        vibes_double_pulse();
    }
}


static void update_time() {
    
    
    
    time_t temp = time(NULL); 
    struct tm *tick_time = localtime(&temp);

    // Create a buffer for the time.
    static char buffer[] = "00:00";
    if(clock_is_24h_style()) {
        strftime(buffer, sizeof("00:00"), "%k:%M", tick_time);
    } else {
        strftime(buffer, sizeof("00:00"), "%l:%M", tick_time);
    }
    text_layer_set_text(layerTime, buffer);
    
    //Get the 24 hour time for time of day checking.
    strftime(tmpTime, sizeof("00"), "%H", tick_time);
    

    // Draw the date, along with the time of day.
    static char date_buffer[] = "  ";
    static char month_buffer[] = "  ";
    static char full_date_buffer[] = "  /  ";
    
    //Get the month and date, along with the weekday
    strftime(month_buffer, sizeof(month_buffer), "%m", tick_time);
    strftime(date_buffer, sizeof(date_buffer), "%e", tick_time);
    strftime(tmpWeekday, sizeof(tmpWeekday), "%w", tick_time);
    
    //Read the first character in the month buffer and see if it is the number 0. If so, get rid of it.
    if ( strncmp(month_buffer,"0",1) == 0 ) {
        strncpy(month_buffer," ",1);
    }
    
    //Copy the month into the full date buffer.
    strcpy(full_date_buffer,month_buffer);
    
    //Add the / to the full buffer.
    strcat(full_date_buffer,"/");
 
    //Add the date to the end as well.
    //Note: checking for a zero in the date is not neccesary, due to it filling the first number with a space anyway.
    strcat(full_date_buffer,date_buffer);
    
    //Set the date text to the full buffer.
    text_layer_set_text(layerDate, full_date_buffer);

    //If the hour has changed.
    if ( strcmp(varTime,tmpTime) != 0 ){
        
        //Store the hour into the variable.
        strcpy(varTime, tmpTime);
        
        //Check the weekday, then set the text accordingly.
        if ( strcmp(tmpWeekday,"0") == 0 ){
            text_layer_set_text(layerWeekday, "Su");
        } else 
        if ( strcmp(tmpWeekday,"1") == 0 ){
            text_layer_set_text(layerWeekday, "M");
        } else 
        if ( strcmp(tmpWeekday,"2") == 0 ){
            text_layer_set_text(layerWeekday, "Tu");
        } else 
        if ( strcmp(tmpWeekday,"3") == 0 ){
            text_layer_set_text(layerWeekday, "W");
        } else 
        if ( strcmp(tmpWeekday,"4") == 0 ){
            text_layer_set_text(layerWeekday, "Th");
        } else 
        if ( strcmp(tmpWeekday,"5") == 0 ){
            text_layer_set_text(layerWeekday, "F");
        } else 
        if ( strcmp(tmpWeekday,"6") == 0 ){
            text_layer_set_text(layerWeekday, "Sa");
        } 
        
        //Check the hour of the day, then set the time of the day image.
        gbitmap_destroy(sprWord);
        
        if ( strcmp(varTime,"00") == 0 || strcmp(varTime,"01") == 0 || strcmp(varTime,"02") == 0 ) {
			sprWord = gbitmap_create_with_resource(RESOURCE_ID_HUD_LATE);        
		} else 
		if ( strcmp(varTime,"03") == 0 || strcmp(varTime,"04") == 0 || strcmp(varTime,"05") == 0 ) {
			sprWord = gbitmap_create_with_resource(RESOURCE_ID_HUD_E_MORNING);
        } else 
		if ( strcmp(varTime,"06") == 0 || strcmp(varTime,"07") == 0 || strcmp(varTime,"08") == 0 ) {
			sprWord = gbitmap_create_with_resource(RESOURCE_ID_HUD_MORNING);
        } else     
		if ( strcmp(varTime,"09") == 0 || strcmp(varTime,"10") == 0 || strcmp(varTime,"11") == 0 ) {
			sprWord = gbitmap_create_with_resource(RESOURCE_ID_HUD_DAYTIME);
        } else     
		if ( strcmp(varTime,"12") == 0 ) {
			sprWord = gbitmap_create_with_resource(RESOURCE_ID_HUD_LUNCH);
        } else       
		if ( strcmp(varTime,"13") == 0 || strcmp(varTime,"14") == 0 || strcmp(varTime,"15") == 0  || strcmp(varTime,"16") == 0  || strcmp(varTime,"17") == 0 ) {
			sprWord = gbitmap_create_with_resource(RESOURCE_ID_HUD_AFTERNOON);
        } else   
		if ( strcmp(varTime,"18") == 0 || strcmp(varTime,"19") == 0 || strcmp(varTime,"20") == 0 ) {
			sprWord = gbitmap_create_with_resource(RESOURCE_ID_HUD_EVENING);
        } else   	
		if ( strcmp(varTime,"21") == 0 || strcmp(varTime,"22") == 0 || strcmp(varTime,"23") == 0 ) {
			sprWord = gbitmap_create_with_resource(RESOURCE_ID_HUD_NIGHT);
        } else {
            sprWord = gbitmap_create_with_resource(RESOURCE_ID_HUD_NIGHT);
        }
        bitmap_layer_set_bitmap(layerWord, sprWord);
    }
}


static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
    update_time();
}

static void main_window_load(Window *window) {

    //Load the background for Aplite, or the files for Colored models.
    #if defined(PBL_BW)
        sprBack = gbitmap_create_with_resource(RESOURCE_ID_HUD_BACK_BW);
        layerBack = bitmap_layer_create(GRect(0, 0, 144, 168));
        bitmap_layer_set_bitmap(layerBack, sprBack);
        layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(layerBack));
    #else
        sprBack = gbitmap_create_with_resource(RESOURCE_ID_TOP_BLUE);
        sprGrey = gbitmap_create_with_resource(RESOURCE_ID_TOP_GREY);
        //sprDark = gbitmap_create_with_resource(RESOURCE_ID_TOP_DARK);
    
        #if defined(PBL_ROUND)
            layerGrey  = bitmap_layer_create(GRect(70, 28, 110, 113));
    
            layerBack  = bitmap_layer_create(GRect(36, 6, 144, 68));
            layerBackC = bitmap_layer_create(GRect(-14, -9, 144, 68));
            
            bitmap_layer_set_compositing_mode(layerBackC, GCompOpSet);
    
            bitmap_layer_set_bitmap(layerBackC, sprBack);
    
            layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(layerBackC));
        #else
            layerGrey = bitmap_layer_create(GRect(34, 22, 110, 113));
            layerBack = bitmap_layer_create(GRect(0, 0, 144, 68));
            
        #endif
    
        bitmap_layer_set_compositing_mode(layerBack, GCompOpSet);
        bitmap_layer_set_compositing_mode(layerGrey, GCompOpSet);
    
        bitmap_layer_set_bitmap(layerBack, sprBack);
        bitmap_layer_set_bitmap(layerGrey, sprGrey);
        
        layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(layerGrey));
        layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(layerBack));
    #endif
    
    
    
    
    // Load the word.
    sprWord = gbitmap_create_with_resource(RESOURCE_ID_HUD_LATE);
    
    #if defined(PBL_COLOR)
        //sprDarkWord = gbitmap_create_with_resource(RESOURCE_ID_DARK_WORD);
        #if defined(PBL_ROUND)
            layerDarkWord = bitmap_layer_create(GRect(10, 15, 144, 168));
        #else
            layerDarkWord = bitmap_layer_create(GRect(0, 0, 144, 168));
        #endif
        bitmap_layer_set_bitmap(layerDarkWord, sprDarkWord);
        bitmap_layer_set_compositing_mode(layerDarkWord, GCompOpSet);
        layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(layerDarkWord));
    #endif
    
    #if defined(PBL_ROUND)
        layerWord = bitmap_layer_create(GRect(10, 15, 144, 168));
    #else
        layerWord = bitmap_layer_create(GRect(0, 0, 144, 168));
    #endif
    
   
    bitmap_layer_set_bitmap(layerWord, sprWord);
    
    
    // Set the compositing mode to allow transparency.
    bitmap_layer_set_compositing_mode(layerWord, GCompOpSet);
    
    layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(layerWord));


    // Load fonts
    fontTime = fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK);
    fontWeekday = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_OS_24));
    fontDate = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_TS_24));

    

    // Load the Moon icon.
    sprMoon = gbitmap_create_with_resource(RESOURCE_ID_HUD_MOON);
    
    #if defined(PBL_ROUND)
        layerMoon = bitmap_layer_create(GRect(38, 4, 144, 168));
    #else
        layerMoon = bitmap_layer_create(GRect(0, 0, 144, 168));
    #endif
    
    bitmap_layer_set_bitmap(layerMoon, sprMoon);
    bitmap_layer_set_compositing_mode(layerMoon, GCompOpSet);
    layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(layerMoon));

    // Create time TextLayer
    #if defined(PBL_ROUND)
        layerTime = text_layer_create(GRect(0, 62, 180, 30));
    #else
        layerTime = text_layer_create(GRect(0, 58, 100, 30));
    #endif
    text_layer_set_background_color(layerTime, GColorClear);
    text_layer_set_text_color(layerTime, GColorLightGray);
    text_layer_set_text(layerTime, "00:00");
    text_layer_set_font(layerTime, fontTime);
    text_layer_set_text_alignment(layerTime, GTextAlignmentCenter);
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(layerTime));

    // Create date TextLayer
    #if defined(PBL_ROUND)
        layerDate = text_layer_create(GRect(0, 4, 180, 30));
        text_layer_set_text_alignment(layerDate, GTextAlignmentCenter);
    #else
        layerDate = text_layer_create(GRect(0, 1, 105, 30));
        text_layer_set_text_alignment(layerDate, GTextAlignmentRight);
    #endif
    
    text_layer_set_text_color(layerDate, GColorBlack);
    text_layer_set_background_color(layerDate, GColorClear);
    text_layer_set_text(layerDate, "  /  ");
    text_layer_set_font(layerDate, fontDate);
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(layerDate));
    
    // Create weekday TextLayer
    #if defined(PBL_ROUND)
        layerWeekday = text_layer_create(GRect(130, 22, 40, 30));
        text_layer_set_text_alignment(layerWeekday, GTextAlignmentLeft);
    #else
        layerWeekday = text_layer_create(GRect(115, 0, 40, 30));
        text_layer_set_text_alignment(layerWeekday, GTextAlignmentLeft);
    #endif
    
    #if defined(PBL_BW)
        text_layer_set_text_color(layerWeekday, GColorBlack);
    #else
        text_layer_set_text_color(layerWeekday, GColorLightGray);
    #endif
    
    text_layer_set_background_color(layerWeekday, GColorClear);
    
    text_layer_set_text(layerWeekday, "NA"); // M Tu W Th F Su Sa
    text_layer_set_font(layerWeekday, fontWeekday);
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(layerWeekday));
    
    // Load the P3 logo.
    sprP3 = gbitmap_create_with_resource(RESOURCE_ID_TOP_P3);
    #if defined(PBL_ROUND)
        layerP3 = bitmap_layer_create(GRect(34, 0, 144, 168));
    #else
        layerP3 = bitmap_layer_create(GRect(0, 0, 144, 168));
    #endif
    bitmap_layer_set_bitmap(layerP3, sprP3);
    bitmap_layer_set_compositing_mode(layerP3, GCompOpSet);
    layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(layerP3));

    // Create circle Layer
    //layerCircle = layer_create(GRect(115, 0, 144, 30));
    //graphics_context_set_fill_color(window_get_root_layer(window), GColorLightGray)
    //graphics_fill_circle(window, GPoint(110,12), 3);
    //layer_add_child(window_get_root_layer(window), text_layer_get_layer(layerCircle));

    // Initialize the display
    update_time();

    bluetooth_callback(connection_service_peek_pebble_app_connection());
}

static void main_window_unload(Window *window) {
    //Unload custom fonts.
    //fonts_unload_custom_font(fontTime);
    fonts_unload_custom_font(fontDate);

    // Unload the images.
    gbitmap_destroy(sprBack);
    gbitmap_destroy(sprWord);
    gbitmap_destroy(sprDarkWord);
    gbitmap_destroy(sprP3);
    gbitmap_destroy(sprMoon);
    
    // Destroy the image layers
    bitmap_layer_destroy(layerBack);
    bitmap_layer_destroy(layerWord);
    bitmap_layer_destroy(layerP3);
    bitmap_layer_destroy(layerDarkWord);
    bitmap_layer_destroy(layerMoon);
    #if defined(PBL_ROUND)
        bitmap_layer_destroy(layerBackC);
    #endif
    
    // Destroy the text layers.
    text_layer_destroy(layerTime);
    text_layer_destroy(layerDate);
    text_layer_destroy(layerWeekday);
    
    // Destroy the Moon layer.
    //layer_destroy(layerBat);
}

static void init() {
    // Pebble-y init stuff.
    windowMain = window_create();
    window_set_window_handlers(windowMain, (WindowHandlers) {
        .load = main_window_load,
        .unload = main_window_unload
    });
    window_set_background_color(windowMain,GColorBlack);
    window_stack_push(windowMain, true);
    tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
    connection_service_subscribe((ConnectionHandlers) {
        .pebble_app_connection_handler = bluetooth_callback
    });
}

static void deinit() {
    window_destroy(windowMain);
}

int main(void) {
    init();
    app_event_loop();
    deinit();
}
