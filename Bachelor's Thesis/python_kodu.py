import tensorflow as tf
import numpy as np
import cv2


model_path = "classifier7.h5"
weights_path = "classifier7.weights.h5"

try:
    model = tf.keras.models.load_model(model_path)
    model.load_weights(weights_path)
    print("✅ Model and weights loaded successfully!")
except Exception as e:
    print(f"❌ Error loading model: {e}")
    exit()


categ = ['Black-grass (wild)', 'Charlock (wild)', 'Cleavers (wild)', 
         'Common Chickweed (wild)', 'Common wheat (non-wild)', 
         'Fat Hen (wild)', 'Loose Silky-bent (wild)', 'Maize (non-wild)',
        'Scentless Mayweed (wild)', 'Shepherds Purse (wild)', 
         'Small-flowered Cranesbill (wild)', 'Sugar beet (non-wild)']

def preprocess_image_from_frame(frame):
    try:
        print("📷 Preprocessing captured image...")
        
        
        resize_img = cv2.resize(frame, (64, 64))
        
        
        Gblur_img = cv2.GaussianBlur(resize_img, (3, 3), 0)
        
        
        hsv_img = cv2.cvtColor(Gblur_img, cv2.COLOR_BGR2HSV) 
        
        
        lower_green = (25, 40, 50)
        upper_green = (75, 255, 255)
        mask = cv2.inRange(hsv_img, lower_green, upper_green)
        
        
        kernel = cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (11, 11))
        mask = cv2.morphologyEx(mask, cv2.MORPH_CLOSE, kernel)
        
        
        bMask = mask > 0
        clearImg = np.zeros_like(resize_img, np.uint8)  
        clearImg[bMask] = resize_img[bMask]  
        
        
        preprocessed_img = clearImg / 255.0
        
        
        img = np.expand_dims(preprocessed_img, axis=0)
        
        print("✅ Preprocessing complete.")
        return img
    except Exception as e:
        print(f"❌ Preprocessing error: {e}")
        return None
    
def predict_and_show(frame):
    print("🧠 Captured image. Running prediction...")
    img = preprocess_image_from_frame(frame)
    if img is None:
        return

    print("🔄 Running model.predict...")
    prediction = model.predict(img, verbose=0)[0]  
    predicted_class = np.argmax(prediction)
    confidence = prediction[predicted_class] * 100

    print(f"✅ Prediction complete!")
    print(f"📌 Predicted category: {categ[predicted_class]}")
    print(f"📊 Confidence: {confidence:.2f}%")

    print("\n📈 Prediction Scores:")
    for i, prob in enumerate(prediction):
        print(f"   {categ[i]:<30}: {prob * 100:.2f}%")

    
    display_img = cv2.resize(frame, (256, 256))
    text = f"{categ[predicted_class]} ({confidence:.2f}%)"
    cv2.putText(display_img, text, (5, 20), cv2.FONT_HERSHEY_SIMPLEX, 
                0.45, (0, 255, 0), 1, cv2.LINE_AA)
    cv2.imshow("Prediction", display_img)



cap = cv2.VideoCapture(0)

print("🎥 Camera started. Press 'c' to capture and classify. Press 'q' to quit.")

while True:
    ret, frame = cap.read()
    if not ret:
        print("❌ Failed to capture from camera.")
        break

    
    small_frame = cv2.resize(frame, (320, 240))
    cv2.imshow("Live Feed", small_frame)

    key = cv2.waitKey(1) & 0xFF

    if key == ord('c'):
        predict_and_show(frame)
    elif key == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()
