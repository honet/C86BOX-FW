
//`#start header` -- edit after this line, do not edit this line
// ========================================
//
// Copyright YOUR COMPANY, THE YEAR
// All Rights Reserved
// UNPUBLISHED, LICENSED SOFTWARE.
//
// CONFIDENTIAL AND PROPRIETARY INFORMATION
// WHICH IS THE PROPERTY OF your company.
//
// ========================================
`include "cypress.v"
//`#end` -- edit above this line, do not edit this line
// Generated on 09/16/2016 at 02:27
// Component: CBusDOut
module CBusDOut (
	input            CLKIN,
	input            reset,
	input            IORDY,
    input            A0,
	output reg [7:0] DOUT_H,
	output reg [7:0] DOUT_L,
	output           BHE,
	output           DOUT_EN,
	output reg       NIOW,
	output           NMWC,
	output           NMWE
);

//`#start body` -- edit after this line, do not edit this line

// ------------------------------------------------
// Parameters
// アクセスウエイト(8MHzで2wait必要)
// 5~6あたりで化ける
parameter [7:0] HiPulseCount = 8'd4;
parameter [7:0] LoPulseCount = 8'd8;

localparam [7:0] LoPulseCountPeriod = LoPulseCount-1;
localparam [7:0] HiPulseCountPeriod = HiPulseCount-1;

// -------------------------------------------------
// Combinatiorial signals
wire z1_detect;      /* zero detect bit used to count hi read pulse      */
wire z0_detect;      /* zero detect bit used to count lo read pulse      */

wire data_empty;     /* Data FIFO empty status signal to internal FSM    */
wire cmd_empty;      /* Command FIFO empty status signal to internal FSM */
wire cmd_not_full;   /* Command FIFO not full status to system bus       */ 
wire data_not_full;  /* Data FIFO not full status to system bus          */
wire [7:0] data_lsb; /* lower part of data bus                           */
wire [7:0] data_msb; /* higher part of data bus                          */


wire is_odd;            /* */
reg cmd_is_byte_access; /* */

wire data_ready;     /* provide data ready status to state machine       */
wire cmd_ready;      /* provide command ready status to state machine    */
wire op_clk;         /* internal clock to drive the component            */

reg oe;

assign is_odd = A0;
assign cmd_ready  = ~cmd_empty;
assign data_ready = ~data_empty;


// -----------------------------------------------------------
// コンポーネント内クロック
cy_psoc3_udb_clock_enable_v1_0 #(.sync_mode(`TRUE)) ClkSync
(
    /* input  */    .clock_in(CLKIN),
    /* input  */    .enable(1'b1),
    /* output */    .clock_out(op_clk)
);

// -------------------------------------------------------
// ステートマシン
localparam [4:0] STATE_IDLE                          = {2'b00, 3'd0}; // 0,0
localparam [4:0] STATE_LOAD_CMD                      = {2'b00, 3'd1}; // 1,1  CMD_FIFOからA0へロード
localparam [4:0] STATE_DECODE_CMD                    = {2'b01, 3'd0}; // 1,0  A0をpoに出力し、それをデコード

localparam [4:0] STATE_WRITE_LOAD_DATA               = {2'b00, 3'd2}; // 0,2  DATA_FIFOからA1へロード
localparam [4:0] STATE_WRITE_SET_DATA                = {2'b00, 3'd3}; // 0,3  A1をpoに出力し、それをDOUTへ設定
localparam [4:0] STATE_WRITE_RELOAD_LO_PULSE_COUNTER = {2'b00, 3'd4}; // 0,4  LOWパルス幅設定値をD0->A0へロード
localparam [4:0] STATE_WRITE_WAIT_LO_PULSE           = {2'b00, 3'd6}; // 0,6  A0ダウンカウントしてLOWパルス幅生成
localparam [4:0] STATE_WRITE_WAIT_IORDY              = {2'b10, 3'd0}; // 2,0  IORDYがアサートされている間待つ
localparam [4:0] STATE_WRITE_RELOAD_HI_PULSE_COUNTER = {2'b00, 3'd5}; // 0,5  HIパルス幅設定値をD1->A1へロード
localparam [4:0] STATE_WRITE_WAIT_HI_PULSE           = {2'b00, 3'd7}; // 0,7  A1ダウンカウントしてHI状態保持

reg [4:0] state;

always @(posedge op_clk or posedge reset)
  begin
    if(reset) begin
      state <= STATE_IDLE;
      cmd_is_byte_access <= 1'd0;
    end else begin
      case(state)
        // ---------------------------
        // IDLE
        STATE_IDLE:
          state <= (IORDY && cmd_ready && data_ready) ? STATE_LOAD_CMD : STATE_IDLE;
        
        // ---------------------------
        // COMMAND
        STATE_LOAD_CMD:
          state <= STATE_DECODE_CMD;
        STATE_DECODE_CMD:
          begin
            cmd_is_byte_access <= data_lsb[1];
            state <= STATE_WRITE_LOAD_DATA;
            //state <= (data_ready) ? STATE_WRITE_LOAD_DATA : STATE_DECODE_CMD;
          end
        
        // ---------------------------
        // WRITE
        STATE_WRITE_LOAD_DATA:
          state <= STATE_WRITE_SET_DATA;
        STATE_WRITE_SET_DATA:
          state <= STATE_WRITE_RELOAD_LO_PULSE_COUNTER;
        STATE_WRITE_RELOAD_LO_PULSE_COUNTER:
          state <= STATE_WRITE_WAIT_LO_PULSE;
        STATE_WRITE_WAIT_LO_PULSE:
          begin
            if (z0_detect)
              //state <= (IORDY) ? STATE_WRITE_RELOAD_HI_PULSE_COUNTER : STATE_WRITE_WAIT_IORDY;
              if (IORDY)
                state <= STATE_WRITE_RELOAD_HI_PULSE_COUNTER;
              else
                state <= STATE_WRITE_WAIT_IORDY;
            else
              state <= STATE_WRITE_WAIT_LO_PULSE;
          end

        STATE_WRITE_WAIT_IORDY:
          begin
            //state <= (IORDY) ? STATE_WRITE_RELOAD_HI_PULSE_COUNTER : STATE_WRITE_WAIT_IORDY;
            if (IORDY)
              state <= STATE_WRITE_RELOAD_HI_PULSE_COUNTER;
          end
        STATE_WRITE_RELOAD_HI_PULSE_COUNTER:
          state <= STATE_WRITE_WAIT_HI_PULSE;
        STATE_WRITE_WAIT_HI_PULSE:
          begin
            //state <= (z1_detect) ? STATE_IDLE : STATE_WRITE_WAIT_HI_PULSE;
            if (z1_detect)
              state <= STATE_IDLE;
          end
        
        default:
          state <= STATE_IDLE;
      endcase
    end
  end


// -----------------------------------------------------
// ステータスレジスタ(CPU間通信用)
wire [7:0] status;

wire busy = ~(state==STATE_IDLE) | cmd_ready;

assign status[3'd0] = busy;   /* transparent */
//assign status[7:1] = 7'd0;   /* unused bits */
assign status[1] = cmd_empty;   /* unused bits */
assign status[2] = data_empty;   /* unused bits */
assign status[7:3] = state;

cy_psoc3_status #(.cy_force_order(1), .cy_md_select(8'h00)) StsReg
(
    /* input          */  .clock(op_clk),
    /* input  [07:00] */  .status(status)
);


// -------------------------------------------
// IO Read/Write 信号生成
// Compute output signals. Change only on positive edge of clock
always @(posedge op_clk)
begin
  NIOW <= ~(state == STATE_WRITE_RELOAD_LO_PULSE_COUNTER | state == STATE_WRITE_WAIT_LO_PULSE | state == STATE_WRITE_WAIT_IORDY);
end


// -------------------------------------------
// データバス出力有効化信号(OUT_EN)生成
always @(posedge op_clk)
begin
  oe <= (state == STATE_WRITE_SET_DATA |
         state == STATE_WRITE_RELOAD_LO_PULSE_COUNTER | state == STATE_WRITE_WAIT_LO_PULSE |
         state == STATE_WRITE_WAIT_IORDY | state == STATE_WRITE_RELOAD_HI_PULSE_COUNTER);
  //     | state == STATE_WRITE_WAIT_HI_PULSE);
end

// --------------------------------------------
// データバス出力データ設定
always @(posedge op_clk)
begin
  if(state == STATE_WRITE_SET_DATA)
    begin
      DOUT_H <= (cmd_is_byte_access) ? (is_odd ? data_lsb : 8'd0) : data_msb;
      DOUT_L <= (cmd_is_byte_access && is_odd) ? data_lsb : data_lsb;
    end
end


// -------------------------------------------
// 出力信号定義
assign DOUT_EN = oe;

assign BHE = ~((state!=STATE_IDLE) && (is_odd || (cmd_is_byte_access==1'd0)));

assign NMWC = 1'b1;
assign NMWE = 1'b1;


//-----------------------------------------
// メモ
//assign SCLK
//8000000kHz =                    125.0ns
//7987200kHz = 2^13×3  ×5^2×13  = 125.2003205128205ns
//  48000kHz = 2^7 ×3  ×5^3     = 
//  44100kHz = 2^2 ×3^2×5^2×7^2 = 

// -------------------------------------
// データバス
wire [14:0] chain;
cy_psoc3_dp #(.d0_init(LoPulseCountPeriod), .d1_init(HiPulseCountPeriod),
.cy_dpconfig(
{
    `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
    `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC_NONE,
    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
    `CS_CMP_SEL_CFGA, /*CFGRAM0: Idle*/
    `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
    `CS_SHFT_OP_PASS, `CS_A0_SRC___F0, `CS_A1_SRC_NONE,
    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
    `CS_CMP_SEL_CFGA, /*CFGRAM1: LOAD_CMD */
    `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
    `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC___F1,
    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
    `CS_CMP_SEL_CFGA, /*CFGRAM2: LOAD_DATA */
    `CS_ALU_OP_PASS, `CS_SRCA_A1, `CS_SRCB_D0,
    `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC_NONE,
    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
    `CS_CMP_SEL_CFGA, /*CFGRAM3:  Idle*/
    `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
    `CS_SHFT_OP_PASS, `CS_A0_SRC___D0, `CS_A1_SRC_NONE,
    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
    `CS_CMP_SEL_CFGA, /*CFGRAM4: LOAD_LO_PULSE */
    `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
    `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC___D1,
    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
    `CS_CMP_SEL_CFGA, /*CFGRAM5: LOAD_HI_PULSE */
    `CS_ALU_OP__DEC, `CS_SRCA_A0, `CS_SRCB_D0,
    `CS_SHFT_OP_PASS, `CS_A0_SRC__ALU, `CS_A1_SRC_NONE,
    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
    `CS_CMP_SEL_CFGA, /*CFGRAM6: COUNT_LO */
    `CS_ALU_OP__DEC, `CS_SRCA_A1, `CS_SRCB_D0,
    `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC__ALU,
    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
    `CS_CMP_SEL_CFGA, /*CFGRAM7: COUNT_HI */
    8'hFF, 8'h00,    /*CFG9: */
    8'hFF, 8'hFF,    /*CFG11-10: */
    `SC_CMPB_A1_D1, `SC_CMPA_A1_D1, `SC_CI_B_ARITH,
    `SC_CI_A_ARITH, `SC_C1_MASK_DSBL, `SC_C0_MASK_DSBL,
    `SC_A_MASK_DSBL, `SC_DEF_SI_0, `SC_SI_B_DEFSI,
    `SC_SI_A_DEFSI, /*CFG13-12: */
    `SC_A0_SRC_ACC, `SC_SHIFT_SL, 1'h0,
    1'h0, `SC_FIFO1_BUS, `SC_FIFO0_BUS,
    `SC_MSB_DSBL, `SC_MSB_BIT0, `SC_MSB_NOCHN,
    `SC_FB_NOCHN, `SC_CMP1_NOCHN,
    `SC_CMP0_NOCHN, /*CFG15-14:             */
    3'h00, `SC_FIFO_SYNC_NONE, 6'h00,
    `SC_FIFO_CLK__DP,`SC_FIFO_CAP_AX,
    `SC_FIFO_LEVEL,`SC_FIFO_ASYNC,`SC_EXTCRC_DSBL,
    `SC_WRK16CAT_DSBL /*CFG17-16:              */
})) Lsb
(
    /* input */          .reset(reset),
    /* input */          .clk(op_clk),
    /* input [02:00] */  .cs_addr(state[2:0]),
    /* input */          .route_si(1'b0),
    /* input */          .route_ci(1'b0),
    /* input */          .f0_load(1'b0),
    /* input */          .f1_load(1'b0),
    /* input */          .d0_load(1'b0),
    /* input */          .d1_load(1'b0),
    /* output */         .ce0(),
    /* output */         .cl0(),
    /* output */         .z0(z0_detect),
    /* output */         .ff0(),
    /* output */         .ce1(),
    /* output */         .cl1(),
    /* output */         .z1(z1_detect),
    /* output */         .ff1(),
    /* output */         .ov_msb(),
    /* output */         .co_msb(),
    /* output */         .cmsb(),
    /* output */         .so(),
    /* output */         .f0_bus_stat(cmd_not_full),
    /* output */         .f0_blk_stat(cmd_empty),
    /* output */         .f1_bus_stat(data_not_full),
    /* output */         .f1_blk_stat(data_empty),
    /* input */          .ci(1'b0),
    /* output */         .co(chain[14]),
    /* input */          .sir(1'b0),
    /* output */         .sor(),
    /* input */          .sil(chain[12]),
    /* output */         .sol(chain[13]),
    /* input */          .msbi(chain[11]),
    /* output */         .msbo(),
    /* input [01:00] */  .cei(2'b0),
    /* output [01:00] */ .ceo(chain[10:9]),
    /* input [01:00] */  .cli(2'b0),
    /* output [01:00] */ .clo(chain[8:7]),
    /* input [01:00] */  .zi(2'b0),
    /* output [01:00] */ .zo(chain[6:5]),
    /* input [01:00] */  .fi(2'b0),
    /* output [01:00] */ .fo(chain[4:3]),
    /* input [01:00] */  .capi(2'b0),
    /* output [01:00] */ .capo(chain[2:1]),
    /* input */          .cfbi(1'b0),
    /* output */         .cfbo(chain[0]),
    /* input [07:00] */  .pi(),
    /* output [07:00] */ .po(data_lsb)
);

cy_psoc3_dp #(.cy_dpconfig(
{
    `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
    `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC_NONE,
    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
    `CS_CMP_SEL_CFGA, /*CFGRAM0: Idle*/
    `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
    `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC_NONE,
    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
    `CS_CMP_SEL_CFGA, /*CFGRAM1: Idle*/
    `CS_ALU_OP_PASS, `CS_SRCA_A1, `CS_SRCB_D0,
    `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC___F1,
    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
    `CS_CMP_SEL_CFGA, /*CFGRAM2: LOAD_DATA */
    `CS_ALU_OP_PASS, `CS_SRCA_A1, `CS_SRCB_D0,
    `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC_NONE,
    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
    `CS_CMP_SEL_CFGA, /*CFGRAM3: Idle*/
    `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
    `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC_NONE,
    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
    `CS_CMP_SEL_CFGA, /*CFGRAM4: Idle*/
    `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
    `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC_NONE,
    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
    `CS_CMP_SEL_CFGA, /*CFGRAM5: Idle*/
    `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
    `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC_NONE,
    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
    `CS_CMP_SEL_CFGA, /*CFGRAM6: Idle*/
    `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
    `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC_NONE,
    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
    `CS_CMP_SEL_CFGA, /*CFGRAM7: Idle*/
    8'hFF, 8'h00,    /*CFG9: */
    8'hFF, 8'hFF,    /*CFG11-10: */
    `SC_CMPB_A1_D1, `SC_CMPA_A1_D1, `SC_CI_B_ARITH,
    `SC_CI_A_ARITH, `SC_C1_MASK_DSBL, `SC_C0_MASK_DSBL,
    `SC_A_MASK_DSBL, `SC_DEF_SI_0, `SC_SI_B_DEFSI,
    `SC_SI_A_DEFSI, /*CFG13-12:              */
    `SC_A0_SRC_ACC, `SC_SHIFT_SL, 1'h0,
    1'h0, `SC_FIFO1_BUS, `SC_FIFO0_BUS,
    `SC_MSB_DSBL, `SC_MSB_BIT0, `SC_MSB_NOCHN,
    `SC_FB_NOCHN, `SC_CMP1_NOCHN,
    `SC_CMP0_NOCHN, /*CFG15-14:             */
    3'h00, `SC_FIFO_SYNC_NONE, 6'h00,
    `SC_FIFO_CLK__DP,`SC_FIFO_CAP_AX,
    `SC_FIFO_LEVEL,`SC_FIFO_ASYNC,`SC_EXTCRC_DSBL,
    `SC_WRK16CAT_DSBL /*CFG17-16:              */
})) Msb
(
    /* input */          .reset(reset),
    /* input */          .clk(op_clk),
    /* input [02:00] */  .cs_addr(state[2:0]),
    /* input */          .route_si(1'b0),
    /* input */          .route_ci(1'b0),
    /* input */          .f0_load(1'b0),
    /* input */          .f1_load(1'b0),
    /* input */          .d0_load(1'b0),
    /* input */          .d1_load(1'b0),
    /* output */         .ce0(),
    /* output */         .cl0(),
    /* output */         .z0(),
    /* output */         .ff0(),
    /* output */         .ce1(),
    /* output */         .cl1(),
    /* output */         .z1(),
    /* output */         .ff1(),
    /* output */         .ov_msb(),
    /* output */         .co_msb(),
    /* output */         .cmsb(),
    /* output */         .so(),
    /* output */         .f0_bus_stat(),
    /* output */         .f0_blk_stat(),
    /* output */         .f1_bus_stat(/*data_not_full*/),
    /* output */         .f1_blk_stat(/*data_empty*/),
    /* input */          .ci(chain[14]),
    /* output */         .co(),
    /* input */          .sir(chain[13]),
    /* output */         .sor(chain[12]),
    /* input */          .sil(1'b0),
    /* output */         .sol(),
    /* input */          .msbi(1'b0),
    /* output */         .msbo(chain[11]),
    /* input [01:00] */  .cei(chain[10:9]),
    /* output [01:00] */ .ceo(),
    /* input [01:00] */  .cli(chain[8:7]),
    /* output [01:00] */ .clo(),
    /* input [01:00] */  .zi(chain[6:5]),
    /* output [01:00] */ .zo(),
    /* input [01:00] */  .fi(chain[4:3]),
    /* output [01:00] */ .fo(),
    /* input [01:00] */  .capi(chain[2:1]),
    /* output [01:00] */ .capo(),
    /* input */          .cfbi(chain[0]),
    /* output */         .cfbo(),
    /* input [07:00] */  .pi(),
    /* output [07:00] */ .po(data_msb)
);


//`#end` -- edit above this line, do not edit this line
endmodule
//`#start footer` -- edit after this line, do not edit this line
//`#end` -- edit above this line, do not edit this line
