#include "imgui_addons.h"

ImVec4 ImAdd::Hex2RGBA(int hexValue, float alpha)
{
    float r = (hexValue >> 16) & 0xFF;
    float g = (hexValue >> 8) & 0xFF;
    float b = hexValue & 0xFF;

    return ImVec4(r / 255, g / 255, b / 255, alpha);
}

bool ImAdd::Button(const char* label, const ImVec2& size_arg, ImDrawFlags drawflag)
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);
    const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);

    ImVec2 pos = window->DC.CursorPos;
    ImVec2 size = ImGui::CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

    const ImRect bb(pos, pos + size);
    ImGui::ItemSize(size, style.FramePadding.y);
    if (!ImGui::ItemAdd(bb, id))
        return false;

    bool hovered, held;
    bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, 0);

    // Animate Colors
    static std::map<ImGuiID, Button_State> anim;
    auto it_anim = anim.find(id);

    if (it_anim == anim.end())
    {
        anim.insert({ id, Button_State() });
        it_anim = anim.find(id);
    }

    it_anim->second.frame_col = ImLerp(it_anim->second.frame_col, (held && hovered) ? ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive) : hovered ? ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered) : ImGui::GetStyleColorVec4(ImGuiCol_Button), g.IO.DeltaTime * fAnimationSpeed);

    // Render
    const ImU32 col = ImGui::GetColorU32(it_anim->second.frame_col);
    ImGui::RenderNavHighlight(bb, id);

    window->DrawList->AddRectFilled(bb.Min, bb.Max, col, style.FrameRounding, drawflag);
    if (style.FrameBorderSize > 0.0f)
    {
        window->DrawList->AddRect(bb.Min + ImVec2(1, 1), bb.Max + ImVec2(1, 1), ImGui::GetColorU32(ImGuiCol_BorderShadow), style.FrameRounding, drawflag, style.FrameBorderSize);
        window->DrawList->AddRect(bb.Min, bb.Max, ImGui::GetColorU32(ImGuiCol_Border), style.FrameRounding, drawflag, style.FrameBorderSize);
    }

    if (g.LogEnabled)
        ImGui::LogSetNextTextDecoration("[", "]");
    ImGui::RenderTextClipped(bb.Min + style.FramePadding, bb.Max - style.FramePadding, label, NULL, &label_size, style.ButtonTextAlign, &bb);

    // Automatically close popups
    //if (pressed && !(flags & ImGuiButtonFlags_DontClosePopups) && (window->Flags & ImGuiWindowFlags_Popup))
    //    CloseCurrentPopup();

    IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags);
    return pressed;
}

bool ImAdd::Selectable(const char* label, bool selected, const ImVec2& size_arg)
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);
    const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);

    ImVec2 pos = window->DC.CursorPos;
    ImVec2 size = ImGui::CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

    const ImRect bb(pos, pos + size);
    ImGui::ItemSize(size, style.FramePadding.y);
    if (!ImGui::ItemAdd(bb, id))
        return false;

    bool hovered, held;
    bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, 0);

    // Animate Colors
    static std::map<ImGuiID, Selectable_State> anim;
    auto it_anim = anim.find(id);

    if (it_anim == anim.end())
    {
        anim.insert({ id, Selectable_State() });
        it_anim = anim.find(id);
    }

    it_anim->second.frame_col = ImLerp(it_anim->second.frame_col, selected ? ImGui::GetStyleColorVec4(ImGuiCol_HeaderActive) : (held && hovered) ? ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive) : hovered ? ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered) : ImGui::GetStyleColorVec4(ImGuiCol_Button), g.IO.DeltaTime * fAnimationSpeed);
    it_anim->second.label_col = ImLerp(it_anim->second.label_col, selected ? ImGui::GetStyleColorVec4(ImGuiCol_Text) : (held && hovered) ? ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled) : hovered ? ImGui::GetStyleColorVec4(ImGuiCol_Text) : ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled), g.IO.DeltaTime * fAnimationSpeed);

    // Render
    const ImU32 frame_col = ImGui::GetColorU32(it_anim->second.frame_col);
    const ImU32 label_col = ImGui::GetColorU32(it_anim->second.label_col);
    ImGui::RenderNavHighlight(bb, id);

    if (selected)
        window->DrawList->AddRectFilled(bb.Min, bb.Max, frame_col, style.FrameRounding, 0);

    if (g.LogEnabled)
        ImGui::LogSetNextTextDecoration("[", "]");

    window->DrawList->AddText(pos + (size / 2) - (label_size / 2), label_col, label);

    // Automatically close popups
    //if (pressed && !(flags & ImGuiButtonFlags_DontClosePopups) && (window->Flags & ImGuiWindowFlags_Popup))
    //    CloseCurrentPopup();

    IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags);
    return pressed;
}

bool ImAdd::RadioButton(const char* label, int current_id, int* id_parent, const ImVec2& size_arg, bool has_separator)
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);
    const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);

    ImVec2 pos = window->DC.CursorPos;
    ImVec2 size = ImGui::CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

    const ImRect bb(pos, pos + size);
    ImGui::ItemSize(size, style.FramePadding.y);
    if (!ImGui::ItemAdd(bb, id))
        return false;

    bool hovered, held;
    bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, 0);

    bool active = *id_parent == current_id;

    if (pressed)
    {
        *id_parent = current_id;
    }

    // Animate Colors
    static std::map<ImGuiID, RadioButton_State> anim;
    auto it_anim = anim.find(id);

    if (it_anim == anim.end())
    {
        anim.insert({ id, RadioButton_State() });
        it_anim = anim.find(id);
    }

    it_anim->second.label_col = ImLerp(it_anim->second.label_col, ImGui::GetStyleColorVec4(hovered ? (active ? ImGuiCol_SliderGrab : ImGuiCol_Text) : active ? ImGuiCol_SliderGrab : ImGuiCol_TextDisabled), g.IO.DeltaTime * fAnimationSpeed);
    
    ImVec4 shadow_rgba = ImGui::GetStyleColorVec4(ImGuiCol_SliderGrab);
    ImVec4 shadow_col = active ? shadow_rgba : hovered ? ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled) : ImVec4(shadow_rgba.x, shadow_rgba.y, shadow_rgba.z, 0.f);
    it_anim->second.shadow_col = ImLerp(it_anim->second.shadow_col, shadow_col, g.IO.DeltaTime * fAnimationSpeed);

    ImGui::RenderNavHighlight(bb, id);

    if (has_separator)
        window->DrawList->AddLine(pos + ImVec2(size.x - 1, style.WindowPadding.y + style.FramePadding.y), pos + ImVec2(size.x - 1, size.y - style.WindowPadding.y - style.FramePadding.y), ImGui::GetColorU32(ImGuiCol_TextDisabled), 1.0f);

    window->DrawList->AddText(pos + size / 2 - label_size / 2, ImGui::GetColorU32(it_anim->second.label_col), label);

    IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags);
    return pressed;
}

bool ImAdd::RadioButtonIcon(const char* icon, const char* str_id, int current_id, int* id_parent, const ImVec2& size_arg)
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(str_id);
    const ImVec2 label_size = ImGui::CalcTextSize(icon, NULL, true);

    ImVec2 pos = window->DC.CursorPos;
    ImVec2 size = ImGui::CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

    const ImRect bb(pos, pos + size);
    ImGui::ItemSize(size, style.FramePadding.y);
    if (!ImGui::ItemAdd(bb, id))
        return false;

    bool hovered, held;
    bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, 0);

    bool active = *id_parent == current_id;

    if (pressed)
    {
        *id_parent = current_id;
    }

    // Animate Colors
    static std::map<ImGuiID, RadioButton_State> anim;
    auto it_anim = anim.find(id);

    if (it_anim == anim.end())
    {
        anim.insert({ id, RadioButton_State() });
        it_anim = anim.find(id);
    }

    it_anim->second.label_col = ImLerp(it_anim->second.label_col, ImGui::GetStyleColorVec4(hovered ? (active ? ImGuiCol_SliderGrab : ImGuiCol_Text) : active ? ImGuiCol_SliderGrab : ImGuiCol_TextDisabled), g.IO.DeltaTime * fAnimationSpeed);

    ImVec4 shadow_rgba = ImGui::GetStyleColorVec4(ImGuiCol_SliderGrab);
    ImVec4 shadow_col = active ? shadow_rgba : hovered ? ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled) : ImVec4(shadow_rgba.x, shadow_rgba.y, shadow_rgba.z, 0.f);
    it_anim->second.shadow_col = ImLerp(it_anim->second.shadow_col, shadow_col, g.IO.DeltaTime * fAnimationSpeed);

    ImGui::RenderNavHighlight(bb, id);

    window->DrawList->AddText(pos + size / 2 - label_size / 2, ImGui::GetColorU32(it_anim->second.label_col), icon);

    window->DrawList->AddShadowCircle(pos + size / 2, size.x / 6, ImGui::GetColorU32(it_anim->second.shadow_col), size.x, ImVec2(0.f, 0.f));

    IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags);
    return pressed;
}

bool ImAdd::Checkbox(const char* label, bool* v)
{
    using namespace ImGui;

    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);
    const ImVec2 label_size = CalcTextSize(label, NULL, true);
    const float width = ImGui::CalcItemSize(ImVec2(-0.1f, 0.f), 0.f, 0.f).x;

    ImVec2 frame_padding = ImVec2(0, 0);
    float height = GetFontSize();

    const float square_sz = GetFontSize() + frame_padding.y * 2;
    const ImVec2 pos = window->DC.CursorPos;
    const ImRect total_bb(pos, pos + ImVec2(width, label_size.y + frame_padding.y * 2.0f));
    ItemSize(total_bb, frame_padding.y);
    if (!ItemAdd(total_bb, id))
    {
        IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags | ImGuiItemStatusFlags_Checkable | (*v ? ImGuiItemStatusFlags_Checked : 0));
        return false;
    }

    bool hovered, held;
    bool pressed = ButtonBehavior(total_bb, id, &hovered, &held);
    if (pressed)
    {
        *v = !(*v);
        MarkItemEdited(id);
    }

    // Animate Colors
    static std::map<ImGuiID, CheckBox_State> anim;
    auto it_anim = anim.find(id);

    if (it_anim == anim.end())
    {
        anim.insert({ id, CheckBox_State() });
        it_anim = anim.find(id);
    }

    ImColor frame_col = ImGui::GetStyleColorVec4(*v ? (held && hovered) ? ImGuiCol_SliderGrabActive : ImGuiCol_SliderGrab : (held && hovered) ? ImGuiCol_FrameBgActive : hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg);
    it_anim->second.frame_col = ImLerp(it_anim->second.frame_col, frame_col, g.IO.DeltaTime * fAnimationSpeed);

    ImVec4 border_rgb = ImGui::GetStyleColorVec4(ImGuiCol_Border);
    ImVec4 border_col = !*v ? border_rgb : ImVec4(border_rgb.x, border_rgb.y, border_rgb.z, 0.0f);
    it_anim->second.border_col = ImLerp(it_anim->second.border_col, border_col, g.IO.DeltaTime * fAnimationSpeed);

    ImVec4 mark_rgb = ImGui::GetStyleColorVec4(ImGuiCol_CheckMark);
    ImVec4 mark_col = *v ? mark_rgb : ImVec4(mark_rgb.x, mark_rgb.y, mark_rgb.z, 0.0f);
    it_anim->second.mark_col = ImLerp(it_anim->second.mark_col, mark_col, g.IO.DeltaTime * fAnimationSpeed);

    // Renders
    const ImRect check_bb(pos + ImVec2(width - height, 0), pos + ImVec2(square_sz, square_sz) + ImVec2(width - height, 0));
    RenderNavHighlight(total_bb, id);

    window->DrawList->AddRectFilled(check_bb.Min, check_bb.Max, GetColorU32(it_anim->second.frame_col), style.FrameRounding);
    const float border_size = g.Style.FrameBorderSize;
    if (border_size > 0.0f)
    {
        window->DrawList->AddRect(check_bb.Min, check_bb.Max, GetColorU32(it_anim->second.border_col), style.FrameRounding, 0, border_size);
    }

    ImU32 check_col = GetColorU32(it_anim->second.mark_col);
    bool mixed_value = (g.LastItemData.InFlags & ImGuiItemFlags_MixedValue) != 0;
    if (mixed_value)
    {
        // Undocumented tristate/mixed/indeterminate checkbox (#2644)
        // This may seem awkwardly designed because the aim is to make ImGuiItemFlags_MixedValue supported by all widgets (not just checkbox)
        ImVec2 pad(ImMax(1.0f, IM_FLOOR(square_sz / 3.6f)), ImMax(1.0f, IM_FLOOR(square_sz / 3.6f)));
        window->DrawList->AddRectFilled(check_bb.Min + pad, check_bb.Max - pad, check_col, style.FrameRounding);
    }
    // Always render the mark
    {
        const float pad = ImMax(1.0f, IM_FLOOR(square_sz / 3.7f));
        RenderCheckMark(window->DrawList, check_bb.Min + ImVec2(pad, pad), check_col, square_sz - pad * 2.0f);
    }

    ImVec2 label_pos = ImVec2(pos.x, check_bb.Min.y + frame_padding.y);
    if (g.LogEnabled)
        LogRenderedText(&label_pos, mixed_value ? "[~]" : *v ? "[x]" : "[ ]");

    if (label_size.x > 0.0f)
        RenderText(label_pos, label);

    IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags | ImGuiItemStatusFlags_Checkable | (*v ? ImGuiItemStatusFlags_Checked : 0));
    return pressed;
}

bool ImAdd::ClassicCheckbox(const char* label, bool* v)
{
    using namespace ImGui;

    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);
    const ImVec2 label_size = CalcTextSize(label, NULL, true);

    ImVec2 frame_padding = ImVec2(0, 0);

    const float square_sz = GetFontSize() + frame_padding.y;
    const ImVec2 pos = window->DC.CursorPos;
    const ImRect total_bb(pos, pos + ImVec2(square_sz + (label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f), label_size.y + frame_padding.y * 2.0f));
    ItemSize(total_bb, frame_padding.y);
    if (!ItemAdd(total_bb, id))
    {
        IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags | ImGuiItemStatusFlags_Checkable | (*v ? ImGuiItemStatusFlags_Checked : 0));
        return false;
    }

    bool hovered, held;
    bool pressed = ButtonBehavior(total_bb, id, &hovered, &held);
    if (pressed)
    {
        *v = !(*v);
        MarkItemEdited(id);
    }

    // Animate Colors
    static std::map<ImGuiID, CheckBox_State> anim;
    auto it_anim = anim.find(id);

    if (it_anim == anim.end())
    {
        anim.insert({ id, CheckBox_State() });
        it_anim = anim.find(id);
    }

    ImColor frame_col = ImGui::GetStyleColorVec4(*v ? (held && hovered) ? ImGuiCol_SliderGrabActive : ImGuiCol_SliderGrab : (held && hovered) ? ImGuiCol_FrameBgActive : hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg);
    it_anim->second.frame_col = ImLerp(it_anim->second.frame_col, frame_col, g.IO.DeltaTime * fAnimationSpeed);

    ImVec4 border_rgb = ImGui::GetStyleColorVec4(ImGuiCol_Border);
    ImVec4 border_col = !*v ? border_rgb : ImVec4(border_rgb.x, border_rgb.y, border_rgb.z, 0.0f);
    it_anim->second.border_col = ImLerp(it_anim->second.border_col, border_col, g.IO.DeltaTime * fAnimationSpeed);

    ImVec4 mark_rgb = ImGui::GetStyleColorVec4(ImGuiCol_CheckMark);
    ImVec4 mark_col = *v ? mark_rgb : ImVec4(mark_rgb.x, mark_rgb.y, mark_rgb.z, 0.0f);
    it_anim->second.mark_col = ImLerp(it_anim->second.mark_col, mark_col, g.IO.DeltaTime * fAnimationSpeed);

    const ImRect check_bb(pos, pos + ImVec2(square_sz, square_sz));
    RenderNavHighlight(total_bb, id);

    window->DrawList->AddRectFilled(check_bb.Min, check_bb.Max, GetColorU32(it_anim->second.frame_col), style.FrameRounding);
    const float border_size = g.Style.FrameBorderSize;
    if (border_size > 0.0f)
    {
        window->DrawList->AddRect(check_bb.Min, check_bb.Max, GetColorU32(it_anim->second.border_col), style.FrameRounding, 0, style.FrameBorderSize);
    }

    ImU32 check_col = GetColorU32(it_anim->second.mark_col);
    bool mixed_value = (g.LastItemData.InFlags & ImGuiItemFlags_MixedValue) != 0;
    if (mixed_value)
    {
        // Undocumented tristate/mixed/indeterminate checkbox (#2644)
        // This may seem awkwardly designed because the aim is to make ImGuiItemFlags_MixedValue supported by all widgets (not just checkbox)
        ImVec2 pad(ImMax(1.0f, IM_FLOOR(square_sz / 3.6f)), ImMax(1.0f, IM_FLOOR(square_sz / 3.6f)));
        window->DrawList->AddRectFilled(check_bb.Min + pad, check_bb.Max - pad, check_col, style.FrameRounding);
    }
    // Always draw
    {
        const float pad = ImMax(1.0f, IM_FLOOR(square_sz / 3.7f));
        RenderCheckMark(window->DrawList, check_bb.Min + ImVec2(pad, pad), check_col, square_sz - pad * 2.0f);
    }

    ImVec2 label_pos = ImVec2(check_bb.Max.x + style.ItemInnerSpacing.x, check_bb.Min.y + frame_padding.y);
    if (g.LogEnabled)
        LogRenderedText(&label_pos, mixed_value ? "[~]" : *v ? "[x]" : "[ ]");
    if (label_size.x > 0.0f)
        RenderText(label_pos, label);

    IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags | ImGuiItemStatusFlags_Checkable | (*v ? ImGuiItemStatusFlags_Checked : 0));
    return pressed;
}

bool ImAdd::ToggleButton(const char* label, bool* v)
{
    using namespace ImGui;

    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);
    const ImVec2 label_size = CalcTextSize(label, NULL, true);

    const float w = ImGui::CalcItemSize(ImVec2(-0.1f, 0.f), 0.f, 0.f).x;

    float height = ImGui::GetFontSize();
    const ImVec2 pos = window->DC.CursorPos;

    float width = height * 2.f;
    float radius = height * 0.50f;

    //const ImRect total_bb(pos, ImVec2(pos.x + width + (label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f), pos.y + label_size.y));
    const ImRect total_bb(pos, pos + ImVec2(w, label_size.y));

    ItemSize(total_bb);
    if (!ItemAdd(total_bb, id))
        return false;

    float last_active_id_timer = g.LastActiveIdTimer;

    bool hovered, held;
    bool pressed = ButtonBehavior(total_bb, id, &hovered, &held);
    if (pressed)
    {
        *v = !(*v);
        MarkItemEdited(id);
        g.LastActiveIdTimer = 0.f;
    }

    if (g.LastActiveIdTimer == 0.f && g.LastActiveId == id && !pressed)
        g.LastActiveIdTimer = last_active_id_timer;

    // Animate Colors
    static std::map<ImGuiID, ToggleButton_State> anim;
    auto it_anim = anim.find(id);

    if (it_anim == anim.end())
    {
        anim.insert({ id, ToggleButton_State() });
        it_anim = anim.find(id);
    }

    ImColor frame_col = ImGui::GetStyleColorVec4(*v ? (held && hovered) ? ImGuiCol_SliderGrabActive : ImGuiCol_SliderGrab : (held && hovered) ? ImGuiCol_FrameBgActive : hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg);
    it_anim->second.frame_col = ImLerp(it_anim->second.frame_col, frame_col, g.IO.DeltaTime * fAnimationSpeed);

    ImVec4 border_rgb = ImGui::GetStyleColorVec4(ImGuiCol_Border);
    ImVec4 border_col = !*v ? border_rgb : ImVec4(border_rgb.x, border_rgb.y, border_rgb.z, 0.0f);
    it_anim->second.border_col = ImLerp(it_anim->second.border_col, border_col, g.IO.DeltaTime * fAnimationSpeed);

    ImVec4 dot_col = ImGui::GetStyleColorVec4((*v || hovered) ? ImGuiCol_Text : ImGuiCol_TextDisabled);
    it_anim->second.dot_col = ImLerp(it_anim->second.dot_col, dot_col, g.IO.DeltaTime * fAnimationSpeed);

    // Anime slide
    float t = *v ? 1.0f : 0.0f;

    if (g.LastActiveId == id)
    {
        float t_anim = ImSaturate(g.LastActiveIdTimer / 0.1f);
        t = *v ? (t_anim) : (1.0f - t_anim);
    }

    const ImRect frame_bb(pos + ImVec2(w - width, 0), ImVec2(pos.x + width, pos.y + height) + ImVec2(w - width, 0));
    float edge_rounding = height * 0.5f;
    window->DrawList->AddRectFilled(frame_bb.Min, frame_bb.Max, GetColorU32(it_anim->second.frame_col), edge_rounding);
    if (!*v && style.FrameBorderSize > 0.0f)
    {
        window->DrawList->AddRect(frame_bb.Min, frame_bb.Max, GetColorU32(GetColorU32(it_anim->second.border_col)), edge_rounding, 0, style.FrameBorderSize);
    }

    RenderNavHighlight(total_bb, id);

    ImVec2 label_pos = ImVec2(pos.x, frame_bb.Min.y);
    RenderText(label_pos, label);
    window->DrawList->AddCircleFilled(ImVec2(pos.x + radius + t * (width - radius * 2.0f), pos.y + radius) + ImVec2(w - width, 0), radius - 4, ImGui::GetColorU32(it_anim->second.dot_col), 36);

    return pressed;
}

bool ImAdd::ClassicToggleButton(const char* label, bool* v)
{
    using namespace ImGui;

    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);
    const ImVec2 label_size = CalcTextSize(label, NULL, true);

    float height = ImGui::GetFontSize();
    const ImVec2 pos = window->DC.CursorPos;

    float width = height * 2.f;
    float radius = height * 0.50f;

    const ImRect total_bb(pos, ImVec2(pos.x + width + (label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f), pos.y + label_size.y));

    ItemSize(total_bb);
    if (!ItemAdd(total_bb, id))
        return false;

    float last_active_id_timer = g.LastActiveIdTimer;

    bool hovered, held;
    bool pressed = ButtonBehavior(total_bb, id, &hovered, &held);
    if (pressed)
    {
        *v = !(*v);
        MarkItemEdited(id);
        g.LastActiveIdTimer = 0.f;
    }

    if (g.LastActiveIdTimer == 0.f && g.LastActiveId == id && !pressed)
        g.LastActiveIdTimer = last_active_id_timer;

    // Animate Colors
    static std::map<ImGuiID, ToggleButton_State> anim;
    auto it_anim = anim.find(id);

    if (it_anim == anim.end())
    {
        anim.insert({ id, ToggleButton_State() });
        it_anim = anim.find(id);
    }

    ImColor frame_col = ImGui::GetStyleColorVec4(*v ? (held && hovered) ? ImGuiCol_SliderGrabActive : ImGuiCol_SliderGrab : (held && hovered) ? ImGuiCol_FrameBgActive : hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg);
    it_anim->second.frame_col = ImLerp(it_anim->second.frame_col, frame_col, g.IO.DeltaTime * fAnimationSpeed);

    ImVec4 border_rgb = ImGui::GetStyleColorVec4(ImGuiCol_Border);
    ImVec4 border_col = !*v ? border_rgb : ImVec4(border_rgb.x, border_rgb.y, border_rgb.z, 0.0f);
    it_anim->second.border_col = ImLerp(it_anim->second.border_col, border_col, g.IO.DeltaTime * fAnimationSpeed);

    ImVec4 dot_col = ImGui::GetStyleColorVec4((*v || hovered) ? ImGuiCol_Text : ImGuiCol_TextDisabled);
    it_anim->second.dot_col = ImLerp(it_anim->second.dot_col, dot_col, g.IO.DeltaTime * fAnimationSpeed);

    // Anime slide
    float t = *v ? 1.0f : 0.0f;

    if (g.LastActiveId == id)
    {
        float t_anim = ImSaturate(g.LastActiveIdTimer / 0.1f);
        t = *v ? (t_anim) : (1.0f - t_anim);
    }

    const ImRect frame_bb(pos, ImVec2(pos.x + width, pos.y + height));
    float edge_rounding = height * 0.5f;
    window->DrawList->AddRectFilled(frame_bb.Min, frame_bb.Max, GetColorU32(it_anim->second.frame_col), edge_rounding);
    if (!*v && style.FrameBorderSize > 0.0f)
    {
        window->DrawList->AddRect(frame_bb.Min, frame_bb.Max, GetColorU32(GetColorU32(it_anim->second.border_col)), edge_rounding, 0, style.FrameBorderSize);
    }

    RenderNavHighlight(total_bb, id);

    ImVec2 label_pos = ImVec2(frame_bb.Max.x + style.ItemInnerSpacing.x, frame_bb.Min.y);
    RenderText(label_pos, label);
    window->DrawList->AddCircleFilled(ImVec2(pos.x + radius + t * (width - radius * 2.0f), pos.y + radius), radius - 4, ImGui::GetColorU32(it_anim->second.dot_col), 36);

    return pressed;
}

bool ImAdd::ColorEdit4(const char* label, float col[4])
{
    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const float width = ImGui::CalcItemSize(ImVec2(-0.1f, 0.f), 0.f, 0.f).x;

    auto pos = ImGui::GetCursorPos();

    std::string str1 = "##";
    std::string str2 = label;

    ImGui::Text(label);

    ImGui::SameLine(pos.x + width - ImGui::GetFrameHeight());

    if (ImGui::ColorButton((str1 + str2).c_str(), ImVec4(col[0], col[1], col[2], col[3]), ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview, { ImGui::GetFrameHeight(), ImGui::GetFontSize() }))
    {
        ImGui::OpenPopup(("COLORPICKER##" + str2).c_str());
    }
    if (ImGui::BeginPopup(("COLORPICKER##" + str2).c_str()))
    {
        {
            ImGui::ColorPicker4(label, col, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoSidePreview);
        }
        ImGui::EndPopup();
    }
}

bool ImAdd::SliderScalar(const char* label, ImGuiDataType data_type, void* p_data, const void* p_min, const void* p_max, const char* format)
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);
    //const float w = ImGui::CalcItemWidth();
    const float w = ImGui::CalcItemSize(ImVec2(-0.1f, 0.f), 0.f, 0.f).x;

    const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);
    const ImRect frame_bb(window->DC.CursorPos + ImVec2(0, ImGui::GetFontSize()), window->DC.CursorPos + ImVec2(0, ImGui::GetFontSize() - 4) + ImVec2(w, label_size.y));
    const ImRect total_bb(frame_bb.Min, frame_bb.Max + ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0.0f));
    const ImRect size_bb(frame_bb.Min, frame_bb.Max + ImVec2(0, ImGui::GetFontSize()));

    const bool temp_input_allowed = false;
    ImGui::ItemSize(size_bb, 0);
    if (!ImGui::ItemAdd(size_bb, id, &frame_bb, temp_input_allowed ? ImGuiItemFlags_Inputable : 0))
        return false;

    // Default format string when passing NULL
    if (format == NULL)
        format = ImGui::DataTypeGetInfo(data_type)->PrintFmt;

    const bool hovered = ImGui::ItemHoverable(frame_bb, id, g.LastItemData.InFlags);
    const bool clicked = hovered && ImGui::IsMouseClicked(0, id);
    const bool make_active = (clicked || g.NavActivateId == id);
    if (make_active && clicked)
        ImGui::SetKeyOwner(ImGuiKey_MouseLeft, id);

    if (make_active)
    {
        ImGui::SetActiveID(id, window);
        ImGui::SetFocusID(id, window);
        ImGui::FocusWindow(window);
        g.ActiveIdUsingNavDirMask |= (1 << ImGuiDir_Left) | (1 << ImGuiDir_Right);
    }

    // Animate Colors
    static std::map<ImGuiID, SliderScalar_State> anim;
    auto it_anim = anim.find(id);

    if (it_anim == anim.end())
    {
        anim.insert({ id, SliderScalar_State() });
        it_anim = anim.find(id);
    }

    it_anim->second.frame_col = ImLerp(it_anim->second.frame_col, ImGui::GetStyleColorVec4(g.ActiveId == id ? ImGuiCol_FrameBgActive : hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg), g.IO.DeltaTime * fAnimationSpeed);
    it_anim->second.line_col = ImLerp(it_anim->second.line_col, ImGui::GetStyleColorVec4(g.ActiveId == id ? ImGuiCol_SliderGrabActive : ImGuiCol_SliderGrab), g.IO.DeltaTime * fAnimationSpeed);
    it_anim->second.grab_col = ImLerp(it_anim->second.grab_col, ImGui::GetStyleColorVec4(g.ActiveId == id ? ImGuiCol_TextDisabled : ImGuiCol_Text), g.IO.DeltaTime * fAnimationSpeed);

    // Draw frame
    ImGui::RenderNavHighlight(frame_bb, id);
    ImGui::RenderFrame(frame_bb.Min + ImVec2(2, 2), frame_bb.Max - ImVec2(2, 2), ImGui::GetColorU32(it_anim->second.frame_col), false, g.Style.FrameRounding);
    window->DrawList->AddRect(frame_bb.Min + ImVec2(2, 2), frame_bb.Max - ImVec2(2, 2), ImGui::GetColorU32(ImGuiCol_Border), style.FrameRounding, 0, style.FrameBorderSize);

    // Slider behavior
    ImRect grab_bb;
    const bool value_changed = ImGui::SliderBehavior(frame_bb, id, data_type, p_data, p_min, p_max, format, 0, &grab_bb);
    if (value_changed)
        ImGui::MarkItemEdited(id);

    float grab_pad = ((frame_bb.Max.y - frame_bb.Min.y) - (grab_bb.Max.y - grab_bb.Min.y)) / 2;

    // Render grab line
    if (grab_bb.Max.x > grab_bb.Min.x)
        window->DrawList->AddRectFilled(frame_bb.Min - ImVec2(0, grab_pad) + ImVec2(0, grab_pad) * 2, grab_bb.Max - ImVec2((grab_bb.Max - grab_bb.Min).y / 2, 0), ImGui::GetColorU32(it_anim->second.line_col), style.GrabRounding);

    // Render grab circle
    if (grab_bb.Max.x > grab_bb.Min.x)
    {
        window->DrawList->AddCircleFilled(grab_bb.Max - ImVec2((grab_bb.Max - grab_bb.Min).y / 2, (grab_bb.Max - grab_bb.Min).y / 2), (frame_bb.Max - frame_bb.Min).y / 2, ImGui::GetColorU32(it_anim->second.grab_col));

        //window->DrawList->AddCircle(grab_bb.Max - ImVec2((grab_bb.Max - grab_bb.Min).y / 2, (grab_bb.Max - grab_bb.Min).y / 2), (frame_bb.Max - frame_bb.Min).y / 2, ImGui::GetColorU32(ImGuiCol_Text));

        window->DrawList->AddCircleFilled(grab_bb.Max - ImVec2((grab_bb.Max - grab_bb.Min).y / 2, (grab_bb.Max - grab_bb.Min).y / 2), grab_pad, ImGui::GetColorU32(ImGuiCol_Text));
    }

    // Display value using user-provided display format so user can add prefix/suffix/decorations to the value.
    char value_buf[64];
    const char* value_buf_end = value_buf + ImGui::DataTypeFormatString(value_buf, IM_ARRAYSIZE(value_buf), data_type, p_data, format);

    ImVec2 value_label_pos = frame_bb.Max - ImVec2(ImGui::CalcTextSize(value_buf).x, ImGui::GetFontSize() * 2);
    window->DrawList->AddText(g.Font, g.FontSize, value_label_pos, ImGui::GetColorU32(ImGuiCol_TextDisabled), value_buf, value_buf_end);

    if (label_size.x > 0.0f)
        ImGui::RenderText(ImVec2(frame_bb.Min.x, frame_bb.Min.y - label_size.y - grab_pad), label);

    IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags | (temp_input_allowed ? ImGuiItemStatusFlags_Inputable : 0));
    return value_changed;
}

bool ImAdd::SliderInt(const char* label, int* v, int v_min, int v_max, const char* format)
{
    return ImAdd::SliderScalar(label, ImGuiDataType_S32, v, &v_min, &v_max, format);
}

bool ImAdd::SliderFloat(const char* label, float* v, float v_min, float v_max, const char* format)
{
    return ImAdd::SliderScalar(label, ImGuiDataType_Float, v, &v_min, &v_max, format);
}

bool ImAdd::KeyBind(const char* label, int* k, float custom_width)
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    ImGuiIO& io = g.IO;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);

    char buf_display[32] = "NONE";
    ImVec2 pos = window->DC.CursorPos;
    const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);
    ImVec2 buf_display_size = ImGui::CalcTextSize(buf_display, NULL, true);
    float width = custom_width == 0 ? ImGui::CalcItemSize(ImVec2(-0.1, 0), 0, 0).x : custom_width;
    float height = ImGui::GetFrameHeight();

    ImVec2 size = ImVec2(width, height);
    ImRect frame_bb(pos + ImVec2(0, label_size.y + style.ItemInnerSpacing.y), pos + size + ImVec2(0, label_size.y + style.ItemInnerSpacing.y));
    ImRect total_bb(pos, frame_bb.Max + ImVec2(size.x, 0));

    ImGui::ItemSize(total_bb, style.FramePadding.y);
    if (!ImGui::ItemAdd(total_bb, id))
        return false;

    const bool hovered = ImGui::ItemHoverable(frame_bb, id, 0);

    if (hovered)
    {
        ImGui::SetHoveredID(id);
        g.MouseCursor = ImGuiMouseCursor_Hand;
    }

    const bool user_clicked = hovered && io.MouseClicked[0];

    if (user_clicked)
    {
        if (g.ActiveId != id)
        {
            memset(io.MouseDown, 0, sizeof(io.MouseDown));
            memset(io.KeysDown, 0, sizeof(io.KeysDown));
            *k = 0;
        }
        ImGui::SetActiveID(id, window);
        ImGui::FocusWindow(window);
    }
    else if (io.MouseClicked[0])
    {
        if (g.ActiveId == id)
            ImGui::ClearActiveID();
    }

    bool value_changed = false;
    int key = *k;

    if (g.ActiveId == id)
    {
        if (!value_changed)
        {
            for (auto i = 0x08; i <= 0xA5; i++)
            {
                if (io.KeysDown[i])
                {
                    key = i;
                    value_changed = true;
                    ImGui::ClearActiveID();
                }
            }
        }

        if (ImGui::IsKeyPressedMap(ImGuiKey_Escape))
        {
            *k = 0;
            ImGui::ClearActiveID();
        }
        else *k = key;
    }

    window->DrawList->AddRectFilled(frame_bb.Min, frame_bb.Max, ImGui::GetColorU32(ImGuiCol_Button), style.FrameRounding);

    const float border_size = g.Style.FrameBorderSize;
    if (border_size > 0.0f)
    {
        window->DrawList->AddRect(frame_bb.Min + ImVec2(1, 1), frame_bb.Max + ImVec2(1, 1), ImGui::GetColorU32(ImGuiCol_BorderShadow), style.FrameBorderSize, 0, border_size);
        window->DrawList->AddRect(frame_bb.Min, frame_bb.Max, ImGui::GetColorU32(ImGuiCol_Border), style.FrameBorderSize, 0, border_size);
    }

    ImGui::RenderNavHighlight(total_bb, id);

    if (*k != 0 && g.ActiveId != id)
        strcpy(buf_display, szKeyNames[*k]);
    else if (g.ActiveId == id)
        strcpy(buf_display, "- Press any key -");

    const ImRect clip_rect(frame_bb.Min.x, frame_bb.Min.y, frame_bb.Min.x + size.x, frame_bb.Min.y + size.y);
    ImGui::RenderTextClipped(frame_bb.Min + style.FramePadding, frame_bb.Max - style.FramePadding, buf_display, NULL, NULL, style.ButtonTextAlign, &clip_rect);

    ImVec2 label_pos = pos;
    if (label_size.x > 0.0f)
        ImGui::RenderText(label_pos, label);

    return value_changed;
}

void ImAdd::SeparatorText(const char* text)
{
    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;

    auto pos = ImGui::GetCursorScreenPos();
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.62f, 0.62f, 0.63f, 1.00f));
    ImGui::Text(text);
    ImGui::PopStyleColor();

    float yOffset = static_cast<int>(ImGui::CalcTextSize(text).y / 2);
    bool bAjustThinkness = (ImGui::CalcTextSize(text).y / 2) - yOffset >= 0.5f ? true : false;

    ImGui::GetWindowDrawList()->AddLine(
        ImVec2(pos.x + ImGui::CalcTextSize(text).x + style.WindowPadding.x,
            pos.y + static_cast<int>(ImGui::CalcTextSize(text).y / 2)),
        ImVec2(pos.x + ImGui::GetWindowSize().x - style.WindowPadding.x * 2,
            pos.y + static_cast<int>(ImGui::CalcTextSize(text).y / 2)),
        ImGui::GetColorU32(ImGuiCol_Border),
        bAjustThinkness ? 2.f : 1.f);

}

bool ImAdd::BeginCombo(const char* label, const char* preview_value, ImGuiComboFlags flags)
{
    using namespace ImGui;

    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();

    ImGuiNextWindowDataFlags backup_next_window_data_flags = g.NextWindowData.Flags;
    g.NextWindowData.ClearFlags(); // We behave like Begin() and need to consume those values
    if (window->SkipItems)
        return false;

    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);
    IM_ASSERT((flags & (ImGuiComboFlags_NoArrowButton | ImGuiComboFlags_NoPreview)) != (ImGuiComboFlags_NoArrowButton | ImGuiComboFlags_NoPreview)); // Can't use both flags together

    ImVec2 pos = window->DC.CursorPos;
    const float arrow_size = (flags & ImGuiComboFlags_NoArrowButton) ? 0.0f : GetFrameHeight();
    const ImVec2 label_size = CalcTextSize(label, NULL, true);
    const float width = ImGui::CalcItemSize(ImVec2(-0.1f, 0.f), 0.f, 0.f).x;
    ImVec2 r_pos = ImVec2(0, label_size.y + style.ItemInnerSpacing.y);

    const ImRect bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(width, label_size.y + style.FramePadding.y * 2.0f));

    const ImRect total_bb(bb.Min, bb.Max + ImVec2(width, label_size.y + style.ItemInnerSpacing.y));
    ItemSize(total_bb, style.FramePadding.y);
    if (!ItemAdd(total_bb, id, &bb))
        return false;

    // Open on click
    bool hovered, held;
    bool pressed = ButtonBehavior(ImRect(total_bb.Min + r_pos, total_bb.Max), id, &hovered, &held);
    const ImGuiID popup_id = ImHashStr("##ComboPopup", 0, id);
    bool popup_open = IsPopupOpen(popup_id, ImGuiPopupFlags_None);
    if (pressed && !popup_open)
    {
        OpenPopupEx(popup_id, ImGuiPopupFlags_None);
        popup_open = true;
    }

    // Animate Colors
    static std::map<ImGuiID, Combo_State> anim;
    auto it_anim = anim.find(id);

    if (it_anim == anim.end())
    {
        anim.insert({ id, Combo_State() });
        it_anim = anim.find(id);
    }

    ImVec4 frame_col = GetStyleColorVec4(hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg);
    it_anim->second.frame_col = ImLerp(it_anim->second.frame_col, frame_col, g.IO.DeltaTime * fAnimationSpeed);

    // Render shape
    const float value_x2 = ImMax(bb.Min.x, bb.Max.x - arrow_size);
    RenderNavHighlight(total_bb, id);

    if (!(flags & ImGuiComboFlags_NoPreview))
        window->DrawList->AddRectFilled(r_pos + bb.Min, r_pos + ImVec2(value_x2, bb.Max.y), GetColorU32(it_anim->second.frame_col), style.FrameRounding, (flags & ImGuiComboFlags_NoArrowButton) ? ImDrawFlags_RoundCornersAll : ImDrawFlags_RoundCornersLeft);

    if (!(flags & ImGuiComboFlags_NoArrowButton))
    {
        ImVec4 bg_col = GetStyleColorVec4((popup_open || hovered) ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
        it_anim->second.bg_col = ImLerp(it_anim->second.bg_col, bg_col, g.IO.DeltaTime * fAnimationSpeed);

        ImVec4 text_col = GetStyleColorVec4(ImGuiCol_Text);
        it_anim->second.text_col = ImLerp(it_anim->second.text_col, text_col, g.IO.DeltaTime * fAnimationSpeed);

        window->DrawList->AddRectFilled(r_pos + ImVec2(value_x2, bb.Min.y), r_pos + bb.Max, GetColorU32(it_anim->second.bg_col), style.FrameRounding, (width <= arrow_size) ? ImDrawFlags_RoundCornersAll : ImDrawFlags_RoundCornersRight);

        if (value_x2 + arrow_size - style.FramePadding.x <= bb.Max.x)
            RenderArrow(window->DrawList, r_pos + ImVec2(value_x2 + style.FramePadding.y, bb.Min.y + style.FramePadding.y), GetColorU32(it_anim->second.text_col), ImGuiDir_Down, 1.0f);
    }

    RenderFrameBorder(r_pos + bb.Min, r_pos + bb.Max, style.FrameRounding);

    // Custom preview
    if (flags & ImGuiComboFlags_CustomPreview)
    {
        g.ComboPreviewData.PreviewRect = ImRect(bb.Min.x, bb.Min.y, value_x2, bb.Max.y);
        IM_ASSERT(preview_value == NULL || preview_value[0] == 0);
        preview_value = NULL;
    }

    // Render preview and label
    if (preview_value != NULL && !(flags & ImGuiComboFlags_NoPreview))
    {
        if (g.LogEnabled)
            LogSetNextTextDecoration("{", "}");
        RenderTextClipped(r_pos + bb.Min + style.FramePadding, r_pos + ImVec2(value_x2, bb.Max.y), preview_value, NULL, NULL);
    }
    if (label_size.x > 0)
        RenderText(ImVec2(pos.x, bb.Min.y), label);

    if (!popup_open)
        return false;

    g.NextWindowData.Flags = backup_next_window_data_flags;
    return ImAdd::BeginComboPopup(popup_id, ImRect(bb.Min + r_pos, bb.Max + r_pos), flags);
}

bool ImAdd::BeginComboPopup(ImGuiID popup_id, const ImRect& bb, ImGuiComboFlags flags)
{
    using namespace ImGui;

    ImGuiContext& g = *GImGui;
    if (!IsPopupOpen(popup_id, ImGuiPopupFlags_None))
    {
        g.NextWindowData.ClearFlags();
        return false;
    }

    // Set popup size
    float w = bb.GetWidth();
    if (g.NextWindowData.Flags & ImGuiNextWindowDataFlags_HasSizeConstraint)
    {
        g.NextWindowData.SizeConstraintRect.Min.x = ImMax(g.NextWindowData.SizeConstraintRect.Min.x, w);
    }
    else
    {
        if ((flags & ImGuiComboFlags_HeightMask_) == 0)
            flags |= ImGuiComboFlags_HeightRegular;
        IM_ASSERT(ImIsPowerOfTwo(flags & ImGuiComboFlags_HeightMask_)); // Only one
        int popup_max_height_in_items = -1;
        if (flags & ImGuiComboFlags_HeightRegular)     popup_max_height_in_items = 8;
        else if (flags & ImGuiComboFlags_HeightSmall)  popup_max_height_in_items = 4;
        else if (flags & ImGuiComboFlags_HeightLarge)  popup_max_height_in_items = 20;
        ImVec2 constraint_min(0.0f, 0.0f), constraint_max(FLT_MAX, FLT_MAX);
        if ((g.NextWindowData.Flags & ImGuiNextWindowDataFlags_HasSize) == 0 || g.NextWindowData.SizeVal.x <= 0.0f) // Don't apply constraints if user specified a size
            constraint_min.x = w;
        if ((g.NextWindowData.Flags & ImGuiNextWindowDataFlags_HasSize) == 0 || g.NextWindowData.SizeVal.y <= 0.0f)
            constraint_max.y = CalcMaxPopupHeightFromItemCount(popup_max_height_in_items);
        SetNextWindowSizeConstraints(constraint_min, constraint_max);
    }

    // This is essentially a specialized version of BeginPopupEx()
    char name[16];
    ImFormatString(name, IM_ARRAYSIZE(name), "##Combo_%02d", g.BeginPopupStack.Size); // Recycle windows based on depth

    // Set position given a custom constraint (peak into expected window size so we can position it)
    // FIXME: This might be easier to express with an hypothetical SetNextWindowPosConstraints() function?
    // FIXME: This might be moved to Begin() or at least around the same spot where Tooltips and other Popups are calling FindBestWindowPosForPopupEx()?
    if (ImGuiWindow* popup_window = FindWindowByName(name))
        if (popup_window->WasActive)
        {
            // Always override 'AutoPosLastDirection' to not leave a chance for a past value to affect us.
            ImVec2 size_expected = CalcWindowNextAutoFitSize(popup_window);
            popup_window->AutoPosLastDirection = (flags & ImGuiComboFlags_PopupAlignLeft) ? ImGuiDir_Left : ImGuiDir_Down; // Left = "Below, Toward Left", Down = "Below, Toward Right (default)"
            ImRect r_outer = GetPopupAllowedExtentRect(popup_window);
            ImVec2 pos = FindBestWindowPosForPopupEx(bb.GetBL(), size_expected, &popup_window->AutoPosLastDirection, r_outer, bb, ImGuiPopupPositionPolicy_ComboBox);
            SetNextWindowPos(pos);
        }

    // We don't use BeginPopupEx() solely because we have a custom name string, which we could make an argument to BeginPopupEx()
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_Popup | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove;
    PushStyleVar(ImGuiStyleVar_WindowPadding, g.Style.FramePadding); // Horizontally align ourselves with the framed text
    bool ret = Begin(name, NULL, window_flags);
    PopStyleVar();
    if (!ret)
    {
        EndPopup();
        IM_ASSERT(0);   // This should never happen as we tested for IsPopupOpen() above
        return false;
    }
    return true;
}

bool ImAdd::Combo(const char* label, int* current_item, bool (*items_getter)(void*, int, const char**), void* data, int items_count, int popup_max_height_in_items)
{
    using namespace ImGui;

    ImGuiContext& g = *GImGui;

    // Call the getter to obtain the preview string which is a parameter to BeginCombo()
    const char* preview_value = NULL;
    if (*current_item >= 0 && *current_item < items_count)
        items_getter(data, *current_item, &preview_value);

    // The old Combo() API exposed "popup_max_height_in_items". The new more general BeginCombo() API doesn't have/need it, but we emulate it here.
    if (popup_max_height_in_items != -1 && !(g.NextWindowData.Flags & ImGuiNextWindowDataFlags_HasSizeConstraint))
        SetNextWindowSizeConstraints(ImVec2(0, 0), ImVec2(FLT_MAX, CalcMaxPopupHeightFromItemCount(popup_max_height_in_items)));

    if (!ImAdd::BeginCombo(label, preview_value, ImGuiComboFlags_None))
        return false;

    // Display items
    // FIXME-OPT: Use clipper (but we need to disable it on the appearing frame to make sure our call to SetItemDefaultFocus() is processed)
    bool value_changed = false;
    for (int i = 0; i < items_count; i++)
    {
        PushID(i);
        const bool item_selected = (i == *current_item);
        const char* item_text;
        if (!items_getter(data, i, &item_text))
            item_text = "*Unknown item*";

        PushStyleVar(ImGuiStyleVar_ItemSpacing, g.Style.FramePadding);
        if (ImAdd::Selectable(item_text, item_selected, ImVec2(-0.1, 0)) && *current_item != i)
        {
            value_changed = true;
            *current_item = i;
        }
        PopStyleVar();

        if (item_selected)
            SetItemDefaultFocus();
        PopID();
    }

    EndCombo();

    if (value_changed)
        MarkItemEdited(g.LastItemData.ID);

    return value_changed;
}

// Combo box helper allowing to pass an array of strings.
bool ImAdd::Combo(const char* label, int* current_item, const char* const items[], int items_count, int height_in_items)
{
    using namespace ImGui;

    const bool value_changed = Combo(label, current_item, Items_ArrayGetter, (void*)items, items_count, height_in_items);
    return value_changed;
}

// Combo box helper allowing to pass all items in a single string literal holding multiple zero-terminated items "item1\0item2\0"
bool ImAdd::Combo(const char* label, int* current_item, const char* items_separated_by_zeros, int height_in_items)
{
    using namespace ImGui;

    int items_count = 0;
    const char* p = items_separated_by_zeros;       // FIXME-OPT: Avoid computing this, or at least only when combo is open
    while (*p)
    {
        p += strlen(p) + 1;
        items_count++;
    }
    bool value_changed = Combo(label, current_item, Items_SingleStringGetter, (void*)items_separated_by_zeros, items_count, height_in_items);
    return value_changed;
}

bool ImAdd::InputText(const char* label, const char* text, char* buf, size_t buf_size, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void* user_data)
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    ImVec2 pos = window->DC.CursorPos;

    char* inv_label;
    inv_label = (char*)malloc(strlen(label) + 3);
    if (inv_label != NULL) {
        strcpy(inv_label, "##");
        strcat(inv_label, label);
    }

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(g.Style.ItemSpacing.x, g.Style.ItemInnerSpacing.y));
    ImGui::Text(label);
    IM_ASSERT(!(flags & ImGuiInputTextFlags_Multiline)); // call InputTextMultiline()
    ImGui::PushItemWidth(ImGui::CalcItemSize(ImVec2(-0.1, 0), 0, 0).x);
    auto result = ImGui::InputTextEx(inv_label, NULL, buf, (int)buf_size, ImVec2(0, 0), flags, callback, user_data);
    ImGui::PopItemWidth();
    ImGui::PopStyleVar();

    if (!ImGui::IsItemActive() && !strlen(buf))
    {
        window->DrawList->AddText(pos + ImVec2(g.Style.FramePadding.x, ImGui::GetFontSize() + g.Style.ItemInnerSpacing.y + g.Style.FramePadding.y), ImGui::GetColorU32(ImGuiCol_TextDisabled), text);
    }

    free(inv_label);
    return result;
}

void ImAdd::RenderFrame(ImVec2 p_min, ImVec2 p_max, ImU32 fill_col, bool border, float rounding, ImDrawFlags draw_flags)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    window->DrawList->AddRectFilled(p_min, p_max, fill_col, rounding, draw_flags);
    const float border_size = g.Style.FrameBorderSize;
    if (border && border_size > 0.0f)
    {
        window->DrawList->AddRect(p_min + ImVec2(1, 1), p_max + ImVec2(1, 1), ImGui::GetColorU32(ImGuiCol_BorderShadow), rounding, draw_flags, border_size);
        window->DrawList->AddRect(p_min, p_max, ImGui::GetColorU32(ImGuiCol_Border), rounding, draw_flags, border_size);
    }
}
