`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 03/20/2019 10:51:22 PM
// Design Name: 
// Module Name: testbenh
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////


module testbenh;

reg clk;
reg aresetn;
reg tready;

initial begin
    clk <= 0;
    aresetn <= 0;
    tready <= 1;
end

wire [63:0] tdata;
wire [63:0] tstrb;
wire tvalid, tlast;
wire [9:0] tmp_count;
wire [63:0] add_num;

my_conuter_v1_0
    #(
        .C_M_AXIS_TDATA_WIDTH(64),
        .C_M_AXIS_START_COUNT(5)
    )
    dut
        (
        .m_axis_aclk(clk),
		.m_axis_aresetn(aresetn),
		.m_axis_tvalid(tvalid),
		.m_axis_tdata(tdata),
		.m_axis_tstrb(tstrb),
		.m_axis_tlast(tlast),
		.m_axis_tready(tready)
    );

always #2.5   clk <= ~clk;

initial begin
    repeat(5) @(posedge clk);
    aresetn <= 1;
    
    repeat(3) @(posedge clk);
    tready <= 1;
    repeat(3000) @(posedge clk);
    $finish;
end
    

endmodule
