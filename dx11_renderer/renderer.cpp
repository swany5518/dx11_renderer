#pragma once

#include "renderer.h"

renderer::renderer(HWND hwnd) :
	default_draw_list(),
	p_swapchain(nullptr),
	p_device_context(nullptr),
	p_backbuffer(nullptr),
	p_layout(nullptr),
	p_vertex_shader(nullptr),
	p_pixel_shader(nullptr),
	p_vertex_buffer(nullptr),
	p_screen_projection_buffer(nullptr),
	screen_projection()
{
	RECT wnd_size{};
	if (!GetClientRect(hwnd, &wnd_size))
		handle_error("renderer - failed to get hwnd window size");

	DXGI_SWAP_CHAIN_DESC swapchain_desc;
	ZeroMemory(&swapchain_desc, sizeof(DXGI_SWAP_CHAIN_DESC));

	swapchain_desc.BufferCount = 1;                                   // one back buffer
	swapchain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;    // use 32-bit color
	swapchain_desc.BufferDesc.Width = wnd_size.right - wnd_size.left; // set the back buffer width
	swapchain_desc.BufferDesc.Height = wnd_size.bottom - wnd_size.top;// set the back buffer height
	swapchain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;     // how swap chain is to be used
	swapchain_desc.OutputWindow = hwnd;                               // the window to be used
	swapchain_desc.SampleDesc.Count = 4;                              // how many multisamples
	swapchain_desc.Windowed = TRUE;                                   // windowed/full-screen mode
	swapchain_desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;    // allow full-screen switching

	if (FAILED(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, NULL, NULL, D3D11_SDK_VERSION, 
		&swapchain_desc, &p_swapchain, &p_device, NULL, &p_device_context)))
		handle_error("renderer - failed to create device and swapchain");

	ID3D11Texture2D* p_backbuffer_texture;
	if (FAILED(p_swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&p_backbuffer_texture)))
		handle_error("renderer - failed to get backbuffer texture");

	if (FAILED(p_device->CreateRenderTargetView(p_backbuffer_texture, NULL, &p_backbuffer)))
		handle_error("renderer - failed to create render target view");

	p_backbuffer_texture->Release();

	p_device_context->OMSetRenderTargets(1, &p_backbuffer, NULL);

	// Set the viewport
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = wnd_size.right - wnd_size.left;
	viewport.Height = wnd_size.bottom - wnd_size.top;

	p_device_context->RSSetViewports(1, &viewport);

	if (FAILED(p_device->CreateVertexShader(shader::vertex, sizeof(shader::vertex), NULL, &p_vertex_shader)))
		handle_error("renderer - failed to create vertex shader");

	if (FAILED(p_device->CreatePixelShader(shader::pixel, sizeof(shader::pixel), NULL, &p_pixel_shader)))
		handle_error("renderer - failed to create pixel shader");

	// set the shader objects
	p_device_context->VSSetShader(p_vertex_shader, 0, 0);
	p_device_context->PSSetShader(p_pixel_shader, 0, 0);

	// create the input layout object
	D3D11_INPUT_ELEMENT_DESC input_elem_desc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	if (FAILED(p_device->CreateInputLayout(input_elem_desc, 2, shader::vertex, sizeof(shader::vertex), &p_layout)))
		handle_error("renderer - failed to create input layout");

	p_device_context->IASetInputLayout(p_layout);

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

	bd.Usage = D3D11_USAGE_DYNAMIC;							// write access access by CPU and GPU
	bd.ByteWidth = sizeof(vertex) * MAX_DRAW_LIST_VERTICES; // size is the VERTEX struct * max vertices
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;				// use as a vertex buffer
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;				// allow CPU to write in buffer

	if (FAILED(p_device->CreateBuffer(&bd, NULL, &p_vertex_buffer)))	// create the buffer
		handle_error("renderer - failed to create vertex buffer");

	UINT stride = sizeof(vertex);
	UINT offset = 0;
	p_device_context->IASetVertexBuffers(0, 1, &p_vertex_buffer, &stride, &offset);
}

renderer::renderer(HWND hwnd, bool should_use_pixel_cords) :
	default_draw_list(),
	p_swapchain(nullptr),
	p_device_context(nullptr),
	p_backbuffer(nullptr),
	p_layout(nullptr),
	p_vertex_shader(nullptr),
	p_pixel_shader(nullptr),
	p_vertex_buffer(nullptr),
	p_screen_projection_buffer(nullptr),
	screen_projection(),
	p_font_factory(nullptr),
	p_font_wrapper(nullptr)
{
	RECT wnd_size{};
	if (!GetClientRect(hwnd, &wnd_size))
		handle_error("renderer - failed to get hwnd window size");

	DXGI_SWAP_CHAIN_DESC swapchain_desc;
	ZeroMemory(&swapchain_desc, sizeof(DXGI_SWAP_CHAIN_DESC));

	swapchain_desc.BufferCount = 1;                                   // one back buffer
	swapchain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;    // use 32-bit color
	swapchain_desc.BufferDesc.Width = wnd_size.right - wnd_size.left; // set the back buffer width
	swapchain_desc.BufferDesc.Height = wnd_size.bottom - wnd_size.top;// set the back buffer height
	swapchain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;     // how swap chain is to be used
	swapchain_desc.OutputWindow = hwnd;                               // the window to be used
	swapchain_desc.SampleDesc.Count = 4;                              // how many multisamples
	swapchain_desc.Windowed = TRUE;                                   // windowed/full-screen mode
	swapchain_desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;    // allow full-screen switching

	if (FAILED(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, NULL, NULL, D3D11_SDK_VERSION,
		&swapchain_desc, &p_swapchain, &p_device, NULL, &p_device_context)))
		handle_error("renderer - failed to create device and swapchain");

	ID3D11Texture2D* p_backbuffer_texture;
	if (FAILED(p_swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&p_backbuffer_texture)))
		handle_error("renderer - failed to get backbuffer texture");

	if (FAILED(p_device->CreateRenderTargetView(p_backbuffer_texture, NULL, &p_backbuffer)))
		handle_error("renderer - failed to create render target view");

	p_backbuffer_texture->Release();

	p_device_context->OMSetRenderTargets(1, &p_backbuffer, NULL);

	// Set the viewport
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = wnd_size.right - wnd_size.left;
	viewport.Height = wnd_size.bottom - wnd_size.top;
	viewport.MinDepth = 0.f;
	viewport.MaxDepth = 1.f;

	p_device_context->RSSetViewports(1, &viewport);

	if (FAILED(p_device->CreateVertexShader(shader::trial_vertex, sizeof(shader::trial_vertex), NULL, &p_vertex_shader)))
		handle_error("renderer - failed to create vertex shader");

	if (FAILED(p_device->CreatePixelShader(shader::trial_pixel, sizeof(shader::trial_pixel), NULL, &p_pixel_shader)))
		handle_error("renderer - failed to create pixel shader");

	// set the shader objects
	p_device_context->VSSetShader(p_vertex_shader, 0, 0);
	p_device_context->PSSetShader(p_pixel_shader, 0, 0);

	// create the input layout object
	D3D11_INPUT_ELEMENT_DESC input_elem_desc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	if (FAILED(p_device->CreateInputLayout(input_elem_desc, 2, shader::vertex, sizeof(shader::vertex), &p_layout)))
		handle_error("renderer - failed to create input layout");

	// create the vertex buffer
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

	bd.Usage = D3D11_USAGE_DYNAMIC;							// write access access by CPU and GPU
	bd.ByteWidth = sizeof(vertex) * MAX_DRAW_LIST_VERTICES; // size is the VERTEX struct * max vertices
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;				// use as a vertex buffer
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;				// allow CPU to write in buffer

	if (FAILED(p_device->CreateBuffer(&bd, NULL, &p_vertex_buffer)))	// create the buffer
		handle_error("renderer - failed to create vertex buffer");

	// create the screen projection buffer
	D3D11_BUFFER_DESC projection_buffer_desc;
	ZeroMemory(&projection_buffer_desc, sizeof(projection_buffer_desc));
	projection_buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
	projection_buffer_desc.ByteWidth = sizeof(DirectX::XMMATRIX);
	projection_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	projection_buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	projection_buffer_desc.MiscFlags = 0;

	if (FAILED(p_device->CreateBuffer(&projection_buffer_desc, nullptr, &p_screen_projection_buffer)))
		handle_error("renderer - failed to create screen projection buffer");

	D3D11_VIEWPORT viewport_test{};
	UINT num_viewports_test = 1;
	p_device_context->RSGetViewports(&num_viewports_test, &viewport_test);
	// calculate the screen projection from the buffer
	screen_projection = DirectX::XMMatrixOrthographicOffCenterLH(viewport.TopLeftX, viewport.Width, viewport.Height, viewport.TopLeftY, viewport.MinDepth, viewport.MaxDepth);

	// map the screen projection in
	D3D11_MAPPED_SUBRESOURCE projection_map_subresource;
	if (FAILED(p_device_context->Map(p_screen_projection_buffer, NULL, D3D11_MAP_WRITE_DISCARD, 0, &projection_map_subresource)))
		handle_error("renderer - failed to map screen projection buffer");

	memcpy(projection_map_subresource.pData, &screen_projection, sizeof(DirectX::XMMATRIX));

	p_device_context->Unmap(p_screen_projection_buffer, NULL);

	// set the screen projection buffer constant
	p_device_context->VSSetConstantBuffers(0, 1, &p_screen_projection_buffer);
	p_device_context->IASetInputLayout(p_layout);

	UINT stride = sizeof(vertex);
	UINT offset = 0;
	p_device_context->IASetVertexBuffers(0, 1, &p_vertex_buffer, &stride, &offset);

	if (FAILED(FW1CreateFactory(FW1_VERSION, &p_font_factory)))
		handle_error("renderer - failed to create font factory");

	if (FAILED(default_draw_list.init_text_geometry(p_font_factory)))
		handle_error("renderer - failed to init text geometry");

	if (FAILED(p_font_factory->CreateFontWrapper(p_device, L"Verdana", &p_font_wrapper)))
		handle_error("renderer - failed to create font wrapper");

	//p_font_wrapper->DrawString(p_device_context, L"", 0.0f, 0.0f, 0.0f, 0xff000000, FW1_RESTORESTATE | FW1_NOFLUSH);
}

renderer::~renderer()
{
	if (p_swapchain)
		p_swapchain->SetFullscreenState(FALSE, NULL);

	safe_release(p_swapchain);
	safe_release(p_device);
	safe_release(p_device_context);
	safe_release(p_backbuffer);
	safe_release(p_layout);
	safe_release(p_vertex_shader);
	safe_release(p_pixel_shader);
	safe_release(p_vertex_buffer);
	safe_release(p_screen_projection_buffer);
	safe_release(p_font_factory);
	safe_release(p_font_wrapper);
}

void renderer::add_vertex(const vertex& vertex, const D3D_PRIMITIVE_TOPOLOGY type)
{
	if (default_draw_list.vertices.size() >= MAX_DRAW_LIST_VERTICES)
		draw();

	if (default_draw_list.batch_list.empty() || default_draw_list.batch_list.back().type != type)
		default_draw_list.batch_list.emplace_back(type, 1);
	else
		default_draw_list.batch_list.back().vertex_count++;

	default_draw_list.vertices.push_back(vertex);
}

void renderer::add_vertices(const vertex* p_vertices, const size_t vertex_count, const D3D_PRIMITIVE_TOPOLOGY type)
{
	if (vertex_count > MAX_DRAW_LIST_VERTICES)
		handle_error("add_vertices - trying to add too many vertices");

	if (default_draw_list.vertices.size() + vertex_count >= MAX_DRAW_LIST_VERTICES)
		draw();

	if (default_draw_list.batch_list.empty() || default_draw_list.batch_list.back().type != type)
		default_draw_list.batch_list.emplace_back(type, vertex_count);
	else
		default_draw_list.batch_list.back().vertex_count += vertex_count;

	// store old size for our memcpy destination address
	auto old_size = default_draw_list.vertices.size();

	// resize our vertex buffer so we can copy the new vertices in
	default_draw_list.vertices.resize(old_size + vertex_count);
	memcpy(&default_draw_list.vertices[old_size], p_vertices, vertex_count * sizeof(vertex));

	// we need to add a vertex between these primitives otherwise it will connect them
	if (type == D3D_PRIMITIVE_TOPOLOGY_LINESTRIP ||
		type == D3D_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ ||
		type == D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP ||
		type == D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ)
		add_vertex({}, D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED);
}

void renderer::draw()
{
	// only draw draw list vertices if size > 0
	if (default_draw_list.vertices.size())
	{
		float background[] = { 0.f, 0.f, 0.f, .0f };
		p_device_context->ClearRenderTargetView(p_backbuffer, background);

		// map our vertex buffer 
		D3D11_MAPPED_SUBRESOURCE mapped_resource;
		if (FAILED(p_device_context->Map(p_vertex_buffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &mapped_resource)))
			return;

		// copy our vertex buffer and unmap
		memcpy(mapped_resource.pData, default_draw_list.vertices.data(), default_draw_list.vertices.size() * sizeof(vertex));
		p_device_context->Unmap(p_vertex_buffer, NULL);

		// iterate each batch and draw it with the respective primitive type
		size_t buffer_index = 0;
		for (auto& batch : default_draw_list.batch_list)
		{
			p_device_context->IASetPrimitiveTopology(batch.type);
			p_device_context->Draw(batch.vertex_count, buffer_index);
			buffer_index += batch.vertex_count;
		}
		default_draw_list.clear();
	}

	p_font_wrapper->Flush(p_device_context);
	p_font_wrapper->DrawGeometry(p_device_context, default_draw_list.p_text_geometry, nullptr, nullptr, FW1_RESTORESTATE);

	p_swapchain->Present(1, 0);
}

void renderer::add_text(const vec2& pos, const std::wstring& text, const color& color, float font_size, uint32_t text_flags)
{
	FW1_RECTF rect{ pos.x, pos.y, pos.x, pos.y };
	text_flags |= FW1_NOFLUSH | FW1_NOWORDWRAP;
	p_font_wrapper->AnalyzeString(nullptr, text.c_str(), L"Verdana", font_size, &rect, 0xFFFFFFFF, text_flags, default_draw_list.p_text_geometry);
}

void renderer::add_line(const vec2& start, const vec2& end, const color& color)
{
	vertex vertices[] =
	{
		vertex{start, color },
		vertex{end,   color },
	};

	add_vertices(vertices, sizeof(vertices) / sizeof(vertex), D3D_PRIMITIVE_TOPOLOGY_LINELIST);
}

void renderer::add_polyline(const vec2* points, size_t size, const color& color)
{
	std::vector<vertex> vertices;
	
	for (auto i = 0u; i < size; ++i)
		vertices.emplace_back(points[i], color);

	add_vertices(vertices.data(), vertices.size(), D3D_PRIMITIVE_TOPOLOGY_LINESTRIP);
}

void renderer::add_line_multicolor(const vec2& start, const color& start_color, const vec2& end, const color& end_color)
{
	vertex vertices[] =
	{
		{start, start_color },
		{end,   end_color   }
	};

	add_vertices(vertices, sizeof(vertices) / sizeof(vertex), D3D_PRIMITIVE_TOPOLOGY_LINELIST);
}

void renderer::add_rect_filled(const vec2& top_left, const vec2& size, const color& color)
{
	vertex vertices[] = 
	{
		{ top_left,                                       color },//{ -0.5f, 0.5f, 0.0f, red },   // top left
		{ vec2{top_left.x + size.x, top_left.y},		  color }, // top right
		{ vec2{top_left.x, top_left.y + size.y},          color },  // bottom_left
		{ vec2{top_left.x + size.x, top_left.y + size.y}, color }, // bottom_right
	};

	add_vertices(vertices, sizeof(vertices) / sizeof(vertex), D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
}

void renderer::add_rect_filled_multicolor(const vec2& top_left, const vec2& size, const color& top_left_color, const color& top_right_color, const color& bottom_left_color, const color& bottom_right_color)
{
	vertex vertices[] =
	{
		{ top_left,                                       top_left_color},//{ -0.5f, 0.5f, 0.0f, red },   // top left
		{ {top_left.x + size.x, top_left.y},			  top_right_color }, // top right
		{ {top_left.x, top_left.y + size.y},          bottom_left_color },  // bottom_left
		{ {top_left.x + size.x, top_left.y + size.y}, bottom_right_color }, // bottom_right
	};

	add_vertices(vertices, sizeof(vertices) / sizeof(vertex), D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
}

void renderer::add_triangle(const vec2& p1, const vec2& p2, const vec2& p3, const color& color)
{
	vertex vertices[] =
	{
		{ p1,  color},
		{ p2,  color},
		{ p3,  color},
		{ p1,  color},
	};

	add_vertices(vertices, sizeof(vertices) / sizeof(vertex), D3D_PRIMITIVE_TOPOLOGY_LINESTRIP);
}

void renderer::add_triangle_filled(const vec2& p1, const vec2& p2, const vec2& p3, const color& color)
{
	// need to arrange filled triangles in clockwise order
	vec2 first{};
	vec2 second{};
	vec2 third{};

	// choose the highest then left most first, then choose right most and highest second to ensure vertices are in clockwise order to get filled
	if (p1.higher_or_leftmost(p2) && p1.higher_or_leftmost(p3))
	{
		first = p1;
		bool p2_is_next = p2.rightmost_or_higher(p3);

		if (p2_is_next)
		{
			second = p2;
			third = p3;
		}
		else
		{
			second = p3;
			third = p2;
		}
	}
	else if (p2.higher_or_leftmost(p3))
	{
		first = p2;
		bool p1_is_next = p1.rightmost_or_higher(p3);
		if (p1_is_next)
		{
			second = p1;
			third = p3;
		}
		else
		{
			second = p3;
			third = p1;
		}
	}
	else
	{
		first = p3;
		bool p1_is_next = p1.rightmost_or_higher(p2);
		if (p1_is_next)
		{
			second = p1;
			third = p2;
		}
		else
		{
			second = p2;
			third = p1;
		}
	}

	vertex vertices[] =
	{
		{ first,  color},
		{ second, color},
		{ third,  color},
	};

	add_vertices(vertices, sizeof(vertices) / sizeof(vertex), D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void renderer::add_triangle_filled_multicolor(const vec2& p1, const vec2& p2, const vec2& p3, const color& p1_color, const color& p2_color, const color& p3_color)
{
	// need to arrange filled triangles in clockwise order
	vec2 first{};
	vec2 second{};
	vec2 third{};

	// choose the highest then left most first, then choose right most and highest second to ensure vertices are in clockwise order to get filled
	if (p1.higher_or_leftmost(p2) && p1.higher_or_leftmost(p3))
	{
		first = p1;
		bool p2_is_next = p2.rightmost_or_higher(p3);

		if (p2_is_next)
		{
			second = p2;
			third = p3;
		}
		else
		{
			second = p3;
			third = p2;
		}
	}
	else if (p2.higher_or_leftmost(p3))
	{
		first = p2;
		bool p1_is_next = p1.rightmost_or_higher(p3);
		if (p1_is_next)
		{
			second = p1;
			third = p3;
		}
		else
		{
			second = p3;
			third = p1;
		}
	}
	else
	{
		first = p3;
		bool p1_is_next = p1.rightmost_or_higher(p2);
		if (p1_is_next)
		{
			second = p1;
			third = p2;
		}
		else
		{
			second = p2;
			third = p1;
		}
	}

	vertex vertices[] =
	{
		{ first,  p1_color},
		{ second, p2_color},
		{ third,  p3_color},
	};

	add_vertices(vertices, sizeof(vertices) / sizeof(vertex), D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void renderer::add_circle(const vec2& middle, float radius, const color& color, size_t segments)
{
	// segment count must be between 4 and MAX_DRAW_LIST_VERTICES
	if (segments < 4 || segments > MAX_DRAW_LIST_VERTICES - 1)
		handle_error("add_circle - need at least 4 and less than MAX_DRAW_LIST_VERTICES");

	// for each circle resolution(segments), we only need to calculate the vertex locations once to avoid calling calc_theta(), sin(), and cos() every call
	static std::unordered_map<size_t, std::vector<vertex>> vertex_cache{};

	// prefill our vertex buffer with segments + 1 vertices
	std::vector<vertex> vertices{};

	auto cached_vertices = vertex_cache.find(segments);

	if (cached_vertices == vertex_cache.end())
	{
		for (auto i = 0u; i <= segments; ++i)
		{
			float theta = calc_theta(i, segments);
			vertices.emplace_back(vec2{ cos(theta), sin(theta) }, color);
		}

		vertex_cache[segments] = vertices;
	}

	else
		vertices = cached_vertices->second;
	
	for (auto& vertex : vertices)
	{
		vertex *= radius;
		vertex += middle;
	}

	add_vertices(vertices.data(), vertices.size(), D3D_PRIMITIVE_TOPOLOGY_LINESTRIP);
}

void renderer::add_circle_filled(const vec2& middle, float radius, const color& color, size_t segments)
{
	// segment count must be between 4 and MAX_DRAW_LIST_VERTICES
	if (segments < 4 || segments > MAX_DRAW_LIST_VERTICES - 1)
		handle_error("add_circle_filled - need at least 4 and less than MAX_DRAW_LIST_VERTICES");

	// for each circle resolution(segments), we only need to calculate the vertex locations once to avoid calling calc_theta(), sin(), and cos() every call
	static std::unordered_map<size_t, std::vector<vertex>> vertex_cache{};

	// declare our vertex list, these will be unit circle coords, multiply by radius and account for middle position to get correct size
	std::vector<vertex> vertices{};

	// check for cached vertices
	auto cached_vertices = vertex_cache.find(segments);

	// if we do not have this circle resolution cached, we need to add it
	if (cached_vertices == vertex_cache.end())
	{
		// vertices 1, 2 and 3 need to be added first
		auto theta_1 = calc_theta(0, segments);
		auto theta_2 = calc_theta(1, segments);
		auto theta_3 = calc_theta(segments - 1, segments);
		
		vertices.emplace_back(vec2{cos(theta_1), sin(theta_1)}, color);
		vertices.emplace_back(vec2{cos(theta_2), sin(theta_2)}, color);
		vertices.emplace_back(vec2{cos(theta_3), sin(theta_3)}, color);

		// for the 4th, 5th, ... nth vertex, its position is dependant on its nth number becuase of trianglestrips
		for (auto list_place = 4u; list_place <= segments; ++list_place)
		{
			// calculate where on the circle the vertex needs to calculated from vertex order for 8 segments the clockwise order is goes 1,2,4,6,8,7,5,3
			auto vertex_n = list_place % 2 != 0 ? segments - list_place / 2 : list_place / 2;
			auto theta_n = calc_theta(vertex_n, segments);
			vertices.emplace_back(vec2{cos(theta_n), sin(theta_n)}, color);
		}

		vertex_cache[segments] = vertices;
	}
	else
		vertices = cached_vertices->second;

	// account for middle position and multiply our unit circle scaled vertices by the radius to get correct size 
	for (auto& vertex : vertices)
	{
		vertex *= radius;
		vertex += middle;
	}
	
	add_vertices(vertices.data(), vertices.size(), D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
}

void renderer::add_frame(const vec2& top_left, const vec2& size, float thickness, const color& frame_color)
{
	// top left -> top right
	add_rect_filled(top_left, { size.x, -thickness }, frame_color);
	// top left -> bottom left
	add_rect_filled(top_left, { thickness, top_left.y - size.y }, frame_color);
	// top right -> bottom right
	add_rect_filled({top_left.x + size.x - thickness, top_left.y}, {thickness, -size.y}, frame_color);
	// bottom left -> bottom right
	add_rect_filled({ top_left.x, top_left.y - size.y + thickness }, {size.x, -thickness}, frame_color);
}

void renderer::add_wire_frame(const vec2& top_left, const vec2& size, const color& frame_color)
{
	vec2 points[] =
	{
		top_left,
		{top_left.x + size.x, top_left.y},
		{top_left.x + size.x, top_left.y + size.y},
		{top_left.x, top_left.y + size.y},
		top_left
	};

	add_polyline(points, sizeof(points) / sizeof(vec2), frame_color);
}

void renderer::add_3d_wire_frame(const vec2& top_left, const vec3& size, const color& frame_color)
{
	vec2 points[] =
	{
		{top_left.x, top_left.y + size.y},						// bottom left
		top_left,												// top left
		{top_left.x + size.x, top_left.y},						// top right
		{top_left.x + size.x, top_left.y + size.y},				// bottom right
		{top_left.x, top_left.y + size.y},						// bottom left
		{top_left.x - size.z, top_left.y + size.y + size.z},	// bottom right - z
		{top_left.x - size.z, top_left.y + size.z},				// top_left - z
		{top_left.x + size.x - size.z, top_left.y + size.z },	// top right - z
		{top_left.x + size.x, top_left.y},						// top right
		top_left,												// top left
		{top_left.x - size.z, top_left.y + size.z},				// top_left - z
	};

	add_polyline(points, sizeof(points) / sizeof(vec2), frame_color);
}

void renderer::add_outlined_frame(const vec2& top_left, const vec2& size, float thickness, float outline_thickness, const color& box_color, const color& outline_color)
{
	
}

void renderer::handle_error(const char* message)
{
	MessageBoxA(NULL, message, "rendering error", MB_ICONERROR);
	exit(-1);
}