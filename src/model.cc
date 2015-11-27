void generic_oscillator::operator()(	const local_state_type &phi, 
										const local_coupling_type &coupling, 
										local_state_type &dphidt)
{

        double V = phi[0];
        double W = phi[1];

        double c_0 = coupling[0];

        double lc_0 = coupling[1] * V; // local_coupling

        dphidt[0] = d * tau * (alpha * W - f * V*V*V + e * V*V + g * V + gamma * I + gamma *c_0 + lc_0);
        dphidt[1] = d * (a + b * V + c * V*V - beta * W) / tau;


}
