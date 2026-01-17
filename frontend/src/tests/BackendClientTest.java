package src.tests;

import src.api.BackendClient;

public class BackendClientTest {

    public static void main(String[] args) {
        System.out.println("Starting Unit Tests...");
        
        try {
            testJsonParsingLogic();
            System.out.println("Test 1 (Parsing): PASSED");
            
            testHealthParsingLogic();
            System.out.println("Test 2 (Health): PASSED");
            
            System.out.println("\nALL TESTS PASSED SUCCESSFULLY!");
        } catch (Exception e) {
            System.err.println("TEST FAILED: " + e.getMessage());
            e.printStackTrace();
        }
    }

    private static void testJsonParsingLogic() throws Exception {
        String mockJson = "{\"prediction\":1, \"confidence\":0.98, \"label\":\"Sigma\"}";
        BackendClient.ClassificationResponse res = BackendClient.ClassificationResponse.fromJson(mockJson);
        
        if (res.prediction != 1) throw new Exception("Prediction mismatch");
        if (res.confidence != 0.98) throw new Exception("Confidence mismatch");
        if (!"Sigma".equals(res.label)) throw new Exception("Label mismatch");
    }

    private static void testHealthParsingLogic() throws Exception {
        String mockHealth = "{\"status\":\"ok\", \"modelLoaded\":true}";
        BackendClient.HealthResponse health = BackendClient.HealthResponse.fromJson(mockHealth);
        
        if (!"ok".equals(health.status)) throw new Exception("Status mismatch");
        if (!health.modelLoaded) throw new Exception("ModelLoaded should be true");
    }
}