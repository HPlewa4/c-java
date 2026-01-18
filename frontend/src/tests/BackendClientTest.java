package src.tests;

import static org.junit.Assert.*;
import org.junit.Test;
import src.api.BackendClient;

public class BackendClientTest {

    
    @Test
    public void testJsonParsingLogic() {
        String mockJson = "{\"prediction\":1, \"confidence\":0.98, \"label\":\"Sigma\"}";
        BackendClient.ClassificationResponse res = BackendClient.ClassificationResponse.fromJson(mockJson);
        assertEquals("Prediction should be 1", 1, res.prediction);
        assertEquals(0.98, res.confidence, 0.001); // Delta for double comparison
        assertEquals("Label should be Sigma", "Sigma", res.label);
    }

    @Test
    public void testHealthParsingLogic() {
        String mockHealth = "{\"status\":\"ok\", \"modelLoaded\":true}";
        BackendClient.HealthResponse health = BackendClient.HealthResponse.fromJson(mockHealth);
        assertEquals("ok", health.status);
        assertTrue("Model should be loaded", health.modelLoaded);
    }

    @Test(expected = NumberFormatException.class)
    public void testMalformedJsonHandling() {
        String badJson = "{invalid:json}";
        BackendClient.ClassificationResponse.fromJson(badJson);
    }
}